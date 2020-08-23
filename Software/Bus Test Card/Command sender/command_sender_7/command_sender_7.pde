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
final boolean useSerial = false;

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

final int LABELS=100;
labelEntry[] labels=new labelEntry[LABELS];
int nextLabel=0;

public class varEntry {
  public String name;
  public int value;
  public int startValue;
  public int stopValue;
  public int increment;
}

final int VARS=100;
varEntry[] vars = new varEntry[VARS];
int nextVar=0;

void saveLabel(String label, int index) {
  
  if (findLabelIndex(label) != -1) { // already in table
    println("saveLabel Duplicate label[" + label + "]");
    exit();
  }

  if (nextLabel == LABELS) {
    println("saveLabel Label table full");
    exit();
  }

  labels[nextLabel] = new labelEntry();
  labels[nextLabel].label=label;
  labels[nextLabel].index =index;
  nextLabel++;
}

int findLabelIndex(String label) {
  int i;

  for (i=0; i<nextLabel; i++) {
    if (labels[i].label.equals(label) == true)
      return(labels[i].index);
  }
  return(-1);
}

void processLabels() {
  String label;
  int i;
  if (verbose >0) println("process labels");
  for (i=0; i < commands.length; i++) {
    if (commands[i].indexOf(":") == 0) { // label
      label = commands[i].substring(1);
      saveLabel(label, i);
    }
  }
}

void dumpLabels() {
  int i;

  for (i=0; i<nextLabel; i++) {
    println("Labeltable[" + i + "] Label[" + labels[i].label + "] index[" + labels[i].index + "]");
  }
  println("End Var Table Dump");
}

void storeVar(String name, int value, int startValue, int stopValue, int increment) {
  int varLocation;

  if ((varLocation=findVarByLabel(name)) != -1) {
    vars[varLocation].value=value;
  } else {
    if (nextVar == VARS) {
      println("storeVar Var table full");
      exit();
    }
    vars[nextVar]= new varEntry();
    vars[nextVar].name=name;
    vars[nextVar].value =value;
    vars[nextVar].startValue=startValue;
    vars[nextVar].stopValue=stopValue;
    vars[nextVar].increment = increment;
    nextVar++;
  }
}

int findVarValue(String name) {
  int i;

  for (i=0; i<nextVar; i++) {
    if (vars[i].name.equals(name) == true) {
      return(vars[i].value);
    }
  }
  println("FindVarValue variable [" + name + "] not found");
  exit();
  return(-1);
}

int findVarByLabel(String label) {
  int i;

  for (i=0; i<nextVar; i++) {
    if (vars[i].name.equals(label) == true) {
      return(i);
    }
  }
  return(-1);
}

void dumpVars() {
  int i;

  for (i=0; i<nextVar; i++) {
    print("Vartable[" + i + "] Var[" + vars[i].name + "]=[" + vars[i].value);
    println("] Stop=[" + vars[i].stopValue + "] Increment=[" +vars[i].increment + "]" );
  }
  println("End Var Table Dump");
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
  processLabels();
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

//dump a range of memory
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

//process a single line of the command file
// Paramter command is the line to process
// Command formats
// 1-"COMMAND:OPERARND# 
// 2-"COMMAND:OPERAND#EXPECTED_RETURN_VALUE!"
// 3-"COMMAND:OPERAND#EXPECTED_RETURN_VALUE!VARIBLE"
// COMMAND=command to be processed by bus driver card
// OPERAND=operand to pass to bus driver card with command
// EXPECTED_REUTRN_VALUE=value expected to be returned by command/operand pair (if applicable) 
//      value returned is text decimal
// VARIABLE=Variable to store returned value in
//
// Paramter match - for commands expecting a return value compare returned result to expected or ignore
int doCommand(String command, boolean match) {
  boolean returnExpected = false;
  int returnValueExpected=0;
  int returnValueActual=-1;
  int numOperand=0;
  String opcode="";
  String operand="";
  String saveName="";
  int varPos;

  if ((varPos=command.indexOf("!")) != -1) { //! indicates a return value is expected
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
    numOperand=findVarValue(command.substring(command.indexOf(":")+2, command.indexOf("#")));
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
      storeVar(saveName, returnValueActual, 0, 0, 0);
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

  if (useSerial) {
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
  String label, varName, tmpString;
  int varValue=0, stopValue=0, increment=0, tmp=0;
  background(111);
  // not needed ??  int newIndex=-1;

  if ((index < commands.length) && (!waitingForInput)) {

    if (verbose!=0) println("Command line #: "+ index+ " Command:[" + commands[index]+"]");

    if (commands[index].indexOf("//") == 0) {  //comment
      println("COMMENT:" + commands[index]);
    } else if (commands[index].indexOf("WAIT") == 0) { // Wait for keyboard or mouse press
      println("Waiting");
      waitingForInput=true;
    } else if (commands[index].indexOf(":") == 0) { // label
      label = commands[index].substring(1);
      if (verbose !=0 ) print ("Label:["+label+"]");
    } else if (commands[index].indexOf("GOTO") == 0) { //  loop back
      label = commands[index].substring(5);
      if (verbose !=0 ) print ("GOTO ["+ label+ "]");
      if ((index=findLabelIndex(label)) == -1) { //was newIndex ? not needed
        println("label not found");
        exit();
      }
    } else if (commands[index].indexOf("LET") == 0) { //  assign value to variable format-LET X=1234
      varName = commands[index].substring(4, commands[index].indexOf("="));
      varValue = int(commands[index].substring(commands[index].indexOf("=")+1));
      if (verbose !=0 ) print ("LET ["+ varName+ "]=" + varValue);
      storeVar(varName, varValue, 0, 0, 0);
    } else if (commands[index].indexOf("FOR") == 0) { //  For loop-FOR A=initial value, final value,increment
      varName = commands[index].substring(4, commands[index].indexOf("="));
      varValue = int(commands[index].substring((commands[index].indexOf("=")+1), commands[index].indexOf(",")));
      tmpString=commands[index].substring(commands[index].indexOf(",")+1);
      stopValue = int(tmpString.substring(0, tmpString.indexOf(",")));
      tmpString=tmpString.substring(tmpString.indexOf(",")+1);
      increment = int(tmpString.substring(tmp));
      if (verbose !=0 ) println("next ["+ varName+ "]=" + varValue + " " +stopValue + " " + increment);
      storeVar(varName, varValue, varValue, stopValue, increment);
      saveLabel("FOR-"+varName, index);
    } else if (commands[index].indexOf("NEXT") == 0) { //  Next - for loop
      varName = commands[index].substring(5);
      if (verbose !=0 ) print ("NEXT ["+ varName+ "]");
      tmp=findVarByLabel(varName);
      vars[tmp].value=vars[tmp].value+vars[tmp].increment;
      if (vars[tmp].increment >0 ) {
        if (vars[tmp].value <= vars[tmp].stopValue) {
          index=findLabelIndex("FOR-"+vars[tmp].name);
        }
      } else {if (vars[tmp].value >= vars[tmp].stopValue) {
          index=findLabelIndex("FOR-"+vars[tmp].name);
        }
      }
    } else if (commands[index].indexOf("DUMPVARS") == 0) { // dump portion of memory
      // not needed ? println("dump " + commands[index].substring(5, 9) +":"+commands[index].substring(10, 14));
      if (verbose!=0) println("Dumpvars");
      dumpVars();
    } else if (commands[index].indexOf("DUMPLABELS") == 0) { // dump portion of memory
      // not needed ? println("dump " + commands[index].substring(5, 9) +":"+commands[index].substring(10, 14));
      if (verbose!=0) println("Dumpvars");
      dumpLabels();
    } else if (commands[index].indexOf("DUMP") == 0) { // dump portion of memory
      // not needed ? println("dump " + commands[index].substring(5, 9) +":"+commands[index].substring(10, 14));
      startAddress  = unhex(commands[index].substring(5, 9));
      endAddress  = unhex(commands[index].substring(10, 14));
      if (verbose!=0) println("Dump:[" + startAddress + "] [" + endAddress + "]");
      dump(startAddress, endAddress);
    } else if (commands[index].length() >0) {
      doCommand(commands[index], true);
    } else {
      println("Empty line");
    }

    delay(delayString);
    /* Not needed 
     if (newIndex !=-1) {
     index = newIndex;
     newIndex=-1;
     }
     */
    index = index + 1;
  } else {
    if (index >= commands.length) {
      println("Test Complete");
      exit();
    }
  }
}
