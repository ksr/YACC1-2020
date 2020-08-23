//import javax.swing.JOptionPane; 
//import javax.swing.JDialog;

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

//import static javax.swing.JOptionPane.*;


final boolean debug = false;
boolean keyPressed=false; // not needed
boolean fileReady=false;
boolean waitingForInput=false;

boolean fileSelected;
String path;

Serial myPort;

int lf = 10;    // Linefeed in ASCII
int cr = 13;    // CR in ASCII
int verbose=0;

int delayString = 50;

String myString = null;
String[] commands;

int index=0;

PFont f; 

void fileSelected(File selection) {
  if (selection == null) {
    println("Window was closed or the user hit cancel.");
    exit();
  } else {
    println("User selected " + selection.getAbsolutePath());
    commands = loadStrings(selection); //read command file
    for (int i=0; i < commands.length; i++)
      println("RAW FILE:" + commands[i]);
    //do_command_file();
  }
  fileReady=true;
  println("File ready");
}

void setup() {
  String COMx, COMlist = "";

  println("Setup Start");
  f = createFont("Arial", 16, true);

  try {
    if (debug) printArray(Serial.list());
    int i = Serial.list().length;
    if (i != 0) {
      if (i >= 2) {
        // need to check which port the inst uses -
        // for now we'll just let the user decide
        for (int j = 0; j < i; ) {
          COMlist += char(j+'a') + " = " + Serial.list()[j];
          if (++j < i) COMlist += ",  ";
        }
        //COMx = showInputDialog("Which COM port is correct? (a,b,..):\n"+COMlist);
        JDialog dialog = new JDialog(); 
        dialog.setAlwaysOnTop(true); 
        COMx =  JOptionPane.showInputDialog(dialog, "Which COM port...\n"+COMlist);
        if (COMx == null) exit();
        if (COMx.isEmpty()) exit();
        i = int(COMx.toLowerCase().charAt(0) - 'a') + 1;
      }
      String portName = Serial.list()[i-1];
      if (debug) println(portName);
      myPort = new Serial(this, portName, 19200); // change baud rate to your liking
      delay(5000);
      while (!isPrompt(getLine()));
      //myPort.bufferUntil('\n'); // buffer until CR/LF appears, but not required..
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
  //selectInput("Select a file to process:", "fileSelected", dataFile("/Users/kennethrother/Documents/YACC1-2020/Software/Bus Test Card/Test driver/tests/Test Vectors/"));
  selectInput("Select a file to process:", "fileSelected", dataFile("../tests/Test Vectors/"));
  //selectInput("Select a file to process:", "fileSelected");
  while (!fileReady)
    delay(25);

  fill(100);
  rect(25, 25, 50, 50);

  println("Setup Complete");
}

int readmem(int addr) {
  int retval=0;

  send_command("WR-ADDRBUS:"+hex(addr, 4)+"#");
  send_command("-MEM-RD:1#");
  retval=send_command("RD-DATABUS-L:0#0!");
  send_command("-MEM-RD:0#");

  return(retval);
}

void dump(int start, int end) {
  int i, memval;

  send_command("-VMA:1#");
  send_command("ADDRBUS-WR-MODE:1#");
  send_command("DATABUS-RD-MODE:1#");

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

int send_command(String com) {

  boolean returnExpected = false;
  int returnValueActual=-1;
  int numOperand;
  String opcode="";
  String operand="";

  //println("send commnad:"+com);
  if (com.indexOf("!") != -1) {
    returnExpected = true;
  }

  opcode=com.substring(0, com.indexOf(":"));
  operand=com.substring(com.indexOf(":")+1, com.indexOf("#"));
  numOperand=unhex(operand);
  //println("Opcode=["+opcode+"] " + "Operand="+operand+" ");

  myPort.write(opcode+":"+numOperand+"#");
  delay(delayString);

  if (returnExpected) {
    String returnString = getLine();
    //println("Return String=[" + returnString+ "]");
    String returnStringActual = returnString.substring(returnString.indexOf(":")+2, returnString.length());
    //println("[" + returnStringActual+ "]");
    returnValueActual = int(returnStringActual);
    //println("Return Actual=[" + hex(returnValueActual, 4) + "]" + " Expected=[" + hex(returnValueExpected, 4)+"]");
  }
  //println("C:"+ getLine()); //complete
  delay(delayString);
  while (!isPrompt(getLine()));
  //println("Received Prompt"); //complete

  return(returnValueActual);
}

void doCommand() {
  boolean returnExpected = false;
  int returnValueExpected=0;
  int returnValueActual=0;
  int numOperand;
  String opcode="";
  String operand="";

  if (commands[index].indexOf("!") != -1) {
    returnExpected = true;
    String returnStringExpected = commands[index].substring(commands[index].indexOf("#")+1, commands[index].indexOf("!"));
    returnValueExpected = unhex(returnStringExpected);
    println("Return expected =[" + hex(returnValueExpected, 4) + "]");
  }

  opcode=commands[index].substring(0, commands[index].indexOf(":"));
  operand=commands[index].substring(commands[index].indexOf(":")+1, commands[index].indexOf("#"));
  numOperand=unhex(operand);
  println("Opcode=["+opcode+"] " + "Operand="+operand+" ");

  myPort.write(opcode+":"+numOperand+"#");
  //myPort.write(commands[index].substring(0, commands[index].indexOf("#")+1));
  delay(delayString);
  //println("Returned["+getLine()+"]"); //command line echo

  if (returnExpected) {
    String returnString = getLine();
    println("Return String=[" + returnString+ "]");
    String returnStringActual = returnString.substring(returnString.indexOf(":")+2, returnString.length());
    //println("[" + returnStringActual+ "]");
    returnValueActual = int(returnStringActual);
    println("Return Actual=[" + hex(returnValueActual, 4) + "]" + " Expected=[" + hex(returnValueExpected, 4)+"]");
    if (returnValueExpected != returnValueActual) {
      println("MisMatch");
      while (true)
        delay(delayString);
    }
  }
  //println("C:"+ getLine()); //complete
  delay(delayString);
  while (!isPrompt(getLine()));
  if (verbose!=0) println("Received Prompt"); //complete
  returnExpected = false;
}



void draw() {
  int startAddress, endAddress;

  background(111);

  if ((index < commands.length) && (!waitingForInput)) {
    if (keyPressed && waitingForInput) {
      keyPressed=false;
      waitingForInput=false;
    }

    if (verbose!=0) println("Command line #: "+ index+ " Command:[" + commands[index]+"]");
    if (commands[index].indexOf("//") == 0) {  
      println("COMMENT:" + commands[index]); //comment
    } else if (commands[index].indexOf("WAIT") == 0) { // Wait for keyboard
      println("Waiting");
      //insert wait for keyboard here
      textFont(f, 16);                  // STEP 3 Specify font to be used
      fill(0);     
      text("Hello Strings!", 10, 10);
      waitingForInput=true;
      keyPressed=false;
    } else if (commands[index].indexOf("DUMP") == 0) {
      println("dump " + commands[index].substring(5, 9) +":"+commands[index].substring(10, 14));
      startAddress  = unhex(commands[index].substring(5, 9));
      endAddress  = unhex(commands[index].substring(10, 14));
      if (verbose!=0) println(startAddress, endAddress);
      dump(startAddress, endAddress);
    } else if (commands[index].length() >0) {
      doCommand();
    } else {
      println("Empty line");
    }

    delay(delayString);
    index = index + 1;
  } else {
    if (index >= commands.length) {
      println("Test Complete");
      exit();
    }
  }
}

void mousePressed() {
  println("In mouse pressed"+key);
  keyPressed=true;
  waitingForInput=false;
}

void keyPressed() {
  println("In key pressed"+key);
  keyPressed=true;
  waitingForInput=false;
}

boolean isPrompt(String line) {
  return(line.equals(">>") == true);
}

String getLine() {

  String tmpString="";
  char c=0;
  boolean done=false;

  while (!done) {
    while (myPort.available() > 0) {
      c = myPort.readChar();
      tmpString += c;  ///ksr test if c = 'cr' return string with no crlf appended
      if (c == lf) {
        return(tmpString.substring(0, tmpString.length()-2));
      } else {
      }
    }
  }
  return("");
}

void do_command_file() {

  println("Start do_command_file:"+commands.length);

  while (index < commands.length) {
    println("Command line #: "+ index+ " Command:[" + commands[index]+"]");

    if (commands[index].indexOf("//") == 0) {  
      println("COMMENT:" + commands[index]); //comment
    } else if (commands[index].indexOf("WAIT") == 0) { // Wait for keyboard
      println("Waiting");
      //insert wait for keyboard here
      while (!keyPressed)
        delay(10);
      keyPressed=false;
    } else if (commands[index].length() >0) {
      doCommand();
    } else {
      println("Empty line");
    }

    delay(delayString);
    index = index + 1;
  }
  println("Test Complete");
  exit();
}
