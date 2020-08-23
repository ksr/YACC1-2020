import processing.serial.*;
import java.io.*;
import java.util.Date;





Serial myPort;
int lf = 10;    // Linefeed in ASCII
int cr = 13;    // CR in ASCII

int delayString = 35;

String myString = null;
String[] commands;

void listDirectories(){
  println("\nListing info about all files in a directory and all subdirectories: ");
  ArrayList<File> allFiles = listFilesRecursive(sketchPath("")+"../../Tests");
  
  for (File f: allFiles) {
    println("Name: " + f.getName());
    println("Full path: " + f.getAbsolutePath());
    //println("Is directory: " + f.isDirectory());
    //println("Is hidden: " + f.isHidden());
    //println("Size: " + f.length());
    //String lastModified = new Date(f.lastModified()).toString();
    //println("Last Modified: " + lastModified);
    println("-----------------------");
  }
}

void setup() {

  println("Start");
  println(dataPath(""));
  println(sketchPath(""));
  //listDirectories();
  


  size(640, 400);
  fill(100);

  
  //commands = loadStrings("../../Tests/commands.txt"); //read command file
  commands = loadStrings("../commands.txt"); //read command file
  //for (int i=0; i < commands.length; i++)
    //println(commands[i]);
  myPort = new Serial(this, "/dev/cu.usbserial-AL00FSLF", 19200);
  delay(2000);
  while (!isPrompt(getLine()));

  println("Setup Complete");
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
    //println("Return expected =[" + returnValueExpected + "]");
  }

  opcode=commands[index].substring(0, commands[index].indexOf(":"));
  operand=commands[index].substring(commands[index].indexOf(":")+1, commands[index].indexOf("#"));
  numOperand=unhex(operand);
  print(operand);
  
  myPort.write(opcode+":"+numOperand+"#");
  //myPort.write(commands[index].substring(0, commands[index].indexOf("#")+1));
  delay(delayString);
  println("A:"+getLine()); //command line echo

  if (returnExpected) {
    String returnString = getLine();
    print("B:=["+ returnString+"] ");
    String returnStringActual = returnString.substring(returnString.indexOf(":")+2, returnString.length());
    //println("[" + returnStringActual+ "]");
    returnValueActual = int(returnStringActual);
    println("Return Actual=[" + hex(returnValueActual,4) + "]" + " Expected=[" + hex(returnValueExpected,4)+"]");
    if (returnValueExpected != returnValueActual) {
      println("MisMatch");
      while (true)
        delay(delayString);
    }
  }
  //println("C:"+ getLine()); //complete
  delay(delayString);
  while (!isPrompt(getLine()));

  returnExpected = false;
}

int keyState = 0;

void mousePressed() {
  println("in keypressed"+key);
}




//String line;
int index=0;
void draw() {

  background(0);
  
  if (index < commands.length) {
    //println("Command: " + commands[index]);

    if (commands[index].indexOf("//") == 0) {  
      println("COMMENT:" + commands[index]); //comment
    } else if (commands[index].indexOf("WAIT") == 0) { // Wait for keyboard
      println("Waiting");
      
      delay(1000);
    } else
      doCommand();

    delay(delayString);
    index = index + 1;
  }
  else{
    println("Test Complete");
    exit();
  }
}


void commandResult(String line) {
  println(line);
}

boolean isError(String line) {
  return(line.equals("Error\r\n") != true);
}

boolean isPrompt(String line) {
  return(line.equals(">>") == true);
}

String getLine() {

  String tmpString="";
  char c=0;
  boolean done=false;

  //println("in getline");

  while (!done) {
    while (myPort.available() > 0) {
      c = myPort.readChar();
      //println("C=["+int(c)+"]");
      tmpString += c;  ///ksr test if c = 'cr' return string with no crlf appended
      if (c == lf) {
        //println("Line done :" + tmpString);
        return(tmpString.substring(0, tmpString.length()-2));
      } else {
      }
    }
  }
  return("");
  /*
  String tmpString = myPort.readStringUntil(lf);
   print("getLine: ");
   print(tmpString);
   return(tmpString);
   */
}

// Function to get a list of all files in a directory and all subdirectories
ArrayList<File> listFilesRecursive(String dir) {
   ArrayList<File> fileList = new ArrayList<File>(); 
   recurseDir(fileList,dir);
   return fileList;
}

// Recursive function to traverse subdirectories
void recurseDir(ArrayList<File> a, String dir) {
  File file = new File(dir);
  if (file.isDirectory()) {
    // If you want to include directories in the list
    //a.add(file);  
    File[] subfiles = file.listFiles();
    for (int i = 0; i < subfiles.length; i++) {
      // Call this function on all files in this directory
      recurseDir(a,subfiles[i].getAbsolutePath());
    }
  } else {
    if (file.isHidden() == false)
    a.add(file);
  }
}