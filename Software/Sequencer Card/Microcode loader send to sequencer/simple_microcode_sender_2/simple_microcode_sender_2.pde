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

Serial myPort;

int lf = 10;    // Linefeed in ASCII
int cr = 13;    // CR in ASCII

int delayString = 25;

final boolean debug = false;
final boolean useSerial = true;

int verbose=1;

String[] commands;
String[] cache;
String[] chache2;

String blank="";

Boolean cacheFile=false;

void setup() {

  String COMx, COMlist = "";

  println("Start");
  println(dataPath(""));
  println(sketchPath(""));

  size(100, 100);
  fill(100);


  commands = loadStrings("../../uCode-Generator2/test.hexz"); //read command file

  cache=loadStrings("../../uCode-Generator2/cache"); //read command file
  if (cache != null) {
    println(" cache file found");
    cacheFile=true;
  } else {
    println("cache file not found");
    cache = loadStrings("../../uCode-Generator2/test.hexz");
    for (int i=0; i < cache.length; i++) {
      cache[i]="";
    }
    println(commands[0]);
    println(cache[0]);
  }
  for (int i=0; i < commands.length; i++) {
    print(i);
    print(" ");
    println(commands[i]);
  }

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
        //while (!isPrompt(getLine())); Not needed i think
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
  //myPort = new Serial(this, "/dev/cu.usbserial-AL00FSLF", 19200);
  //myPort = new Serial(this, "/dev/cu.usbserial-FTGNNBHP", 19200);
  //delay(2000);
  //while (!isPrompt(getLine()));

  println("Setup Complete");
}

int doCommand(int index) {

  String instruction ="";
  String checksum="";
  String data="";

  print("in doCommand instruction=");
  println(index);

  if (commands[index].indexOf("!") == 0) {
    myPort.write("!");
    return(0);
  }

  if (commands[index].indexOf("%") != 0) {
    doError("Line did not start with %");
  }
  if (cacheFile)
    if (commands[index].equals(cache[index])==true) {
      return(1);
    }

  while (!isPrompt(getLine()));
  println("in write");
  checksum=commands[index].substring(1, 3);
  instruction=commands[index].substring(3, 5);
  data=commands[index].substring(5, commands[index].indexOf("-"));

  //sendinfo
  for (int i=0; i <commands[index].indexOf("-"); i++) {
    myPort.write(commands[index].substring(i, i+1));
    delay(delayString);
  }
  cache[index] = commands[index];
  delay(5);
  return(1);
}


int index=0;
void draw() {

  //print("top index -"); 
  //println(index);

  background(555+index*20);

  //while (!isPrompt(getLine()));
  if (index < commands.length) {
    println("Command: " + commands[index]);
    if (doCommand(index) == 0) {
      saveStrings("../../uCode-Generator/cache", cache); //read command file
      println("Test Complete");
      exit();
    }
    delay(delayString);
    index = index + 1;
  }
}

void doError(String msg) {
  println(msg);
  exit();
}

boolean isPrompt(String line) {
  return(line.equals(">>") == true);
}

String getLine() {

  String tmpString="";
  char c=0;
  boolean done=false;

  while (!done) {
    c = getChar();
    tmpString += c;  ///ksr test if c = 'cr' return string with no crlf appended
    if (c == lf) {
      //println("Line done :" + tmpString);
      return(tmpString.substring(0, tmpString.length()-2));
    }
  }

  return("");
}

char getChar() {

  //println("in getChar");
  while (myPort.available() <= 0)
    delay(35);
  char c = myPort.readChar();
  //println("C=["+int(c)+"]["+c+"]");
  return(c);
}
