import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JButton;
import javax.swing.JToggleButton;
import java.awt.Color;
import javax.swing.JTabbedPane;
import processing.serial.*;
import java.io.*;
import java.util.Date;
import javax.swing.*; 
import java.util.*;

final boolean debug = false;
boolean fileReady=false;
boolean waitingForInput=false;

boolean fileSelected;
String path;

Serial myPort;

int lf = 10;    // Linefeed in ASCII
int cr = 13;    // CR in ASCII

int verbose=0  ;

int delayString = 50;

String[] commands;

PFont f;

int index=0;

int command_count=0;

public class labelEntry {
  public String label;
  public int index;
}

labelEntry[] labels=new labelEntry[100];
int nextLabel=0;

public class varEntry {
  public String label;
  public int value;
}

varEntry[] vars = new varEntry[100];
int nextVar=0;

void saveLabel(String label, int index) {
  if (findLabel(label) != -1) // already in table
    return;
  labels[nextLabel] = new labelEntry();
  labels[nextLabel].label=label;
  labels[nextLabel].index =index;
  nextLabel++;
}

int findLabel(String label) {
  int i;
  for (i=0; i<nextLabel; i++) {
    if (labels[i].label.equals(label) == true)
      return(labels[i].index);
  }
  return(-1);
}

void saveVar(String label, int val) {
  int varLocation;

  println("saveVar "+ label + " "+val);

  if ((varLocation=findVarLabel(label)) != -1) {
    vars[varLocation].value=val;
  } else {
    vars[nextVar]= new varEntry();
    vars[nextVar].label=label;
    vars[nextVar].value =val;
    nextVar++;
  }
}

int findVar(String label) {
  int i;

  println("findVar "+ label );

  for (i=0; i<nextVar; i++) {
    if (vars[i].label.equals(label) == true) {
      println("findVar "+ vars[i].value );
      return(vars[i].value);
    }
  }
  return(-1);
}

int findVarLabel(String label) {
  int i;

  println("findLabel "+ label );

  for (i=0; i<nextVar; i++) {
    if (vars[i].label.equals(label) == true) {
      println("findLabel "+i);
      return(i);
    }
  }
  return(-1);
}

void mousePressed() {
  if (verbose>0)
    println("In mouse pressed"+key);
  waitingForInput=false;
}

void keyPressed() {
  if (verbose>0)
    println("In key pressed"+key);
  waitingForInput=false;
}

boolean isPrompt(String line) {
  return(line.equals(">>") == true);
}

String getLine() {
  String tmpString="";
  char c=0;

  while (true) {
    while (myPort.available() > 0) {
      c = myPort.readChar();
      tmpString += c;  ///ksr test if c = 'cr' return string with no crlf appended
      if (c == lf) {
        return(tmpString.substring(0, tmpString.length()-2));
      }
    }
  }
}


void fileSelected(File selection) {
  if (selection == null) {
    println("Window was closed or the user hit cancel.");
    exit();
  } else {
    if (verbose > 0) 
      println("User selected " + selection.getAbsolutePath());
    commands = loadStrings(selection); //read command file
    if (verbose > 0)
      for (int i=0; i < commands.length; i++)
        println("RAW FILE:" + commands[i]);
  }
  fileReady=true;
  if (verbose > 0) 
    println("File ready");
}


//read a single memory location, assumes address is set to write and databus is set to read
int readmem(int addr) {
  int retval=0;

  doCommand("WR-ADDRBUS:"+hex(addr, 4)+"#", false);
  doCommand("-MEM-RD:1#", false);
  retval=doCommand("RD-DATABUS-L:0#0!", false);
  doCommand("-MEM-RD:0#", false);
  return(retval);
}

void dump(int start, int end) {
  int i, memval;

  doCommand("-VMA:1#", false);
  doCommand("ADDRBUS-WR-MODE:1#", false);
  doCommand("DATABUS-RD-MODE:1#", false);

  print(hex(start, 4)+": ");
  for (i=start; i<=end; i++) {
    memval=readmem(i);
    print(hex(memval, 2)+ " ");
    if ((i & 0x0F) == 0x0F) {
      println();
      if ((i+1) <= end) print(hex(i+1, 4)+": ");
    }
  }
  println("");
}

int doCommand(String command, boolean match) {
  boolean returnExpected = false;
  int returnValueExpected=0;
  int returnValueActual=-1;
  int numOperand=0;
  String opcode="";
  String operand="";
  String saveName="";
  int varPos;

  if ((varPos=command.indexOf("!")) != -1) {
    returnExpected = true;
    println("found var at- " + varPos + " " + command.length());
    if ((command.length()-1) != varPos) {
      saveName= command.substring(command.indexOf("!")+1);
    } else {
      saveName="";
    }
    if (match) {
      String returnStringExpected = command.substring(command.indexOf("#")+1, command.indexOf("!"));
      returnValueExpected = unhex(returnStringExpected);
      if (verbose > 0)
        println("Return expected =[" + hex(returnValueExpected, 4) + "]");
    }
  }

  opcode=command.substring(0, command.indexOf(":"));
  operand=command.substring(command.indexOf(":")+1, command.indexOf("#"));
  if (operand.substring(0, 1).equals("V") ==true) {
    println("VAR- ", command.substring(command.indexOf(":")+2, command.indexOf("#")));
    numOperand=findVar(command.substring(command.indexOf(":")+2, command.indexOf("#")));
  } else {
    numOperand=unhex(operand);
    print("["+hex(command_count++, 4)+"] ");
    println("Opcode=["+opcode+"] " + "Operand=["+operand+"]");
  }

  myPort.write(opcode+":"+numOperand+"#");
  delay(delayString);

  if (returnExpected) {
    String returnString = getLine();
    if (verbose > 1) println("Return String=[" + returnString+ "]");
    String returnStringActual = returnString.substring(returnString.indexOf(":")+2, returnString.length());
    returnValueActual = int(returnStringActual);
    if (saveName !="" ) {
      saveVar(saveName, returnValueActual);
    }
    if ((returnValueExpected != returnValueActual) && match && saveName.length()==0) {
      println("Return Actual=[" + hex(returnValueActual, 4) + "]" + " Expected=[" + hex(returnValueExpected, 4)+"]");
      println("MisMatch");
      exit();
    } else {
      println("Match=[" + hex(returnValueActual, 4) +"]" );
    }
  }
  delay(delayString); // needed?
  while (!isPrompt(getLine()));
  if (verbose!=0) println("Received Prompt"); //complete
  returnExpected = false;
  return(returnValueActual);
}

void setup() {
  String COMx, COMlist = "";

  println("Setup Start");
  size(500, 500);
  //rect(25, 25, 50, 50);
  //printArray(PFont.list());
  background(255);
  fill(0);
  f = createFont("Arial", 16, true);
  textFont(f, 16);
  text("hello world", 10, 10);
  delay(1000);

  try {
    if (verbose > 0) 
      printArray(Serial.list());

    int i = Serial.list().length;
    if (i != 0) {
      if (i >= 2) {
        // need to check which port the inst uses -
        // for now we'll just let the user decide
        for (int j = 0; j < i; ) {
          COMlist += char(j+'a') + " = " + Serial.list()[j];
          if (++j < i) COMlist += ",  ";
        }
        JDialog dialog = new JDialog(); 
        dialog.setAlwaysOnTop(true); 
        COMx =  JOptionPane.showInputDialog(dialog, "Which COM port...\n"+COMlist);
        if (COMx == null) exit();
        if (COMx.isEmpty()) exit();
        i = int(COMx.toLowerCase().charAt(0) - 'a') + 1;
      }
      String portName = Serial.list()[i-1];
      if (verbose > 1) println(portName);
      myPort = new Serial(this, portName, 19200); // change baud rate to your liking
      delay(5000);
      while (!isPrompt(getLine()));
    } else {
      JOptionPane.showMessageDialog(frame, "Device is not connected to the PC");
      exit();
    }
  }
  catch (Exception e)
  { //Print the type of error
    JOptionPane.showMessageDialog(frame, "COM port is not available (may\nbe in use by another program)");
    println("Error:", e);
    exit();
  }

  println("select file");
  selectInput("Select a file to process:", "fileSelected", dataFile("../tests/Test Vectors/"));

  while (!fileReady)
    delay(25);

  index=0;

  println("Setup Complete");
}

void draw() {
  int startAddress, endAddress;
  String label;
  background(111);
  int newIndex=-1;

  if ((index < commands.length) && (!waitingForInput)) {

    if (verbose!=0) println("Command line #: "+ index+ " Command:[" + commands[index]+"]");
    if (commands[index].indexOf("//") == 0) {  
      println("COMMENT:" + commands[index]); //comment
    } else if (commands[index].indexOf("WAIT") == 0) { // Wait for keyboard
      println("Waiting");
      text("Hello Strings!", 10, 10);
      waitingForInput=true;
    } else if (commands[index].indexOf(":") == 0) { // label
      if (verbose !=0 ) print ("Label:[");
      label = commands[index].substring(1);
      println(label+"]");
      saveLabel(label, index);
    } else if (commands[index].indexOf("GOTO") == 0) { //  loop back
      if (verbose !=0 ) print ("GOTO [");
      label = commands[index].substring(5);
      println(label+"]");
      if ((newIndex=findLabel(label)) == -1) {
        println("label not found");
        exit();
      }
    } else if (commands[index].indexOf("DUMP") == 0) {
      println("dump " + commands[index].substring(5, 9) +":"+commands[index].substring(10, 14));
      startAddress  = unhex(commands[index].substring(5, 9));
      endAddress  = unhex(commands[index].substring(10, 14));
      if (verbose!=0) println(startAddress, endAddress);
      dump(startAddress, endAddress);
    } else if (commands[index].length() >0) {
      doCommand(commands[index], true);
    } else {
      println("Empty line");
    }

    delay(delayString);
    if (newIndex !=-1) {
      index = newIndex;
      newIndex=-1;
    }
    index = index + 1;
  } else {
    if (index >= commands.length) {
      println("Test Complete");
      exit();
    }
  }
}
