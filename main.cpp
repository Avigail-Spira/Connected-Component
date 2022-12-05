#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;
 
class CCLabel {
public:
  int numRows, numCols, minVal, maxVal;
  int newMin, newMax, newLabel;
  int trueNumCC;
  int** zeroFramedAry;
  int nonZeroNeighborAry[5]; 
  int* EQAry;
 
  class Property {
  public:
    int label, numPixels, minR, minC, maxR, maxC;
  };
 
  Property* CCProperty; 
 
  CCLabel(ifstream &inFile) {
    std::string line;
    getline(inFile, line);
    stringstream ssin(line);
    ssin >> numRows >> numCols >> minVal >> maxVal;
 
    newLabel = 0;
    
    zeroFramedAry = new int* [numRows+2];
    for (int i = 0; i < numRows + 2; i++) {
      zeroFramedAry[i] = new int[numCols+2];
    }
    
    EQAry = new int[(numRows * numCols)/4];
    for (int i = 0; i < (numRows * numCols)/4; i++) {
      EQAry[i] = i;
    }
  } 
 
  ~CCLabel() {
    for (int i = 0; i < numRows + 2; i++) {
      delete[] zeroFramedAry[i];
    }
    delete[] zeroFramedAry;
    delete[] EQAry;
  }
 
  void zero2D(){
    for (int i=0; i<numRows+2; i++) {
      for (int j=0; j<numCols+2; j++) {
          zeroFramedAry[i][j] = 0;
      }
    }
  }
 
  void minus1D(int array[], int n) {
    for (int i = 0; i < n; i++) {
      array[i] = 0;
    }
  }
 
  void loadImage(ifstream &inFile) {
    int row = 1;
    std::string line;
    while (std::getline(inFile, line)) {
      stringstream ssin(line);
      int number;
      for (int i = 1; i < numCols+1; i++) {
        ssin >> number;
        zeroFramedAry[row][i] = number;
      }
      row++;
    }
  }
  
  void imgReformat(ofstream &outFile) {
    string str = to_string(maxVal);
    int width = str.length();
    int r = 0;
 
    while (r < numRows+2) {
      int c = 0;
      while (c < numCols+2) {
        if (zeroFramedAry[r][c] == 0) {
          outFile << ". ";
        } else {
          outFile << zeroFramedAry[r][c] << " ";
        }
        str = to_string(zeroFramedAry[r][c]);
        int ww = str.length();
  
        while (ww <= width) {
            outFile << " ";
            ww++;
        }
        c++;
      }
      outFile << endl;
      r++;
    }
 
    outFile << endl;
  }
 
  int nonZeroMin(int array[], int n) {
    int index = 1;
    int min = array[0];
    while (array[index] != 0 && index < n) {
      if (array[index] < min)
        min = array[index];
      index++;
    }
    return min;
  }
 
  bool equals(int array[], int n) {
    int index = 0;
    while (array[index+1] != 0 && index < n) {
      if (array[index] != array[index+1])
        return false;
      index++;
    }
    return true;
  }
 
  void connect8Pass1() {
    for (int i=1; i<numRows+2; i++) {
      for (int j=1; j<numCols+2; j++) {

        if (zeroFramedAry[i][j] > 0) {

          minus1D(nonZeroNeighborAry, 5);

          int index = 0, col_limit = 0;          
          for (int row = i-1; row <= i; row++) {
            for (int col = j-1; col <= j+1; col++) {
              if (zeroFramedAry[row][col] != 0)
                nonZeroNeighborAry[index++] = zeroFramedAry[row][col];
              
              col_limit++;
              if (col_limit == 4)
                break;
            }
          }         
        
          if (nonZeroNeighborAry[0] == 0 && nonZeroNeighborAry[1] == 0 && nonZeroNeighborAry[2] == 0 && nonZeroNeighborAry[3] == 0) {
            newLabel++;
            zeroFramedAry[i][j] = newLabel;
            updateEQ(newLabel);
          } else if (equals(nonZeroNeighborAry, 5)) {
            zeroFramedAry[i][j] = nonZeroNeighborAry[0];
          } else {
            int min = nonZeroMin(nonZeroNeighborAry, 5);
            zeroFramedAry[i][j] = min;
            updateEQ(min);
          }
        }
      }
    }
  }
 
  void connect8Pass2() {
    for (int i=numRows+1; i>0; i--) {
      for (int j=numCols+1; j>0; j--) {
        if (zeroFramedAry[i][j] > 0) {

          minus1D(nonZeroNeighborAry, 5);
          
          int index = 0, col_limit = 0;
          nonZeroNeighborAry[index++] = zeroFramedAry[i][j];
          for (int row = i+1; row >= i; row--) {
            for (int col = j+1; col >= j-1; col--) {
              if (zeroFramedAry[row][col] != 0)
                nonZeroNeighborAry[index++] = zeroFramedAry[row][col];
              
              col_limit++;
              if (col_limit == 4)
                break;
            }
          } 
        
          if ((nonZeroNeighborAry[1] == 0 && nonZeroNeighborAry[2] == 0 && nonZeroNeighborAry[3] == 0 && nonZeroNeighborAry[4] == 0) || equals(nonZeroNeighborAry, 5)) {
              ;
          } else { 
            int min = nonZeroMin(nonZeroNeighborAry, 5);
            if (zeroFramedAry[i][j] > min) {
              EQAry[zeroFramedAry[i][j]] = min;
              zeroFramedAry[i][j] = min; 
            }
          }
          zeroFramedAry[i][j] = EQAry[zeroFramedAry[i][j]];
        }
      }
    }
  }
  
  void connect4Pass1(){
    for (int i=1; i<numRows+2; i++) {
      for (int j=1; j<numCols+2; j++) {
        if (zeroFramedAry[i][j] > 0) {

          minus1D(nonZeroNeighborAry, 5);
          int index = 0, col_limit = 0;
          for (int row = i-1; row <= i; row++) {
            for (int col = j; col >= j-1; col--) {
              if ((row != i || col != j) && zeroFramedAry[row][col] != 0)
                nonZeroNeighborAry[index++] = zeroFramedAry[row][col];
              
              col_limit++;
              if (col_limit == 1)
                break;
            }
          }  
 
          if (nonZeroNeighborAry[0] == 0 && nonZeroNeighborAry[1] == 0) {
              newLabel++;
              zeroFramedAry[i][j] = newLabel;
              updateEQ(newLabel);
          } else if (equals(nonZeroNeighborAry, 5)) {
            zeroFramedAry[i][j] = nonZeroNeighborAry[0];
          } else {
            int min = nonZeroMin(nonZeroNeighborAry, 5);
            zeroFramedAry[i][j] = min;
            updateEQ(min);
          }
        }
      }
    }
  }
  
  void connect4Pass2(){
    for (int i=numRows+1; i>0; i--) {
      for (int j=numCols+1; j>0; j--) {
        if (zeroFramedAry[i][j] > 0) {

          minus1D(nonZeroNeighborAry, 5);
          int index = 0, col_limit = 0;
          nonZeroNeighborAry[index++] = zeroFramedAry[i][j];
          for (int row = i; row <= i+1; row++) {
            for (int col = j+1; col >= j; col--) {
              if ((row != i+1 || col != j+1) && zeroFramedAry[row][col] != 0)
                nonZeroNeighborAry[index++] = zeroFramedAry[row][col];
              
              col_limit++;
              if (col_limit == 1)
                break;
            }
          }
        
          if ((nonZeroNeighborAry[1] == 0 && nonZeroNeighborAry[2] == 0) || equals(nonZeroNeighborAry, 5)) {
              ;
          } else { 
            int min = nonZeroMin(nonZeroNeighborAry, 5);
            if (zeroFramedAry[i][j] > min) {
              EQAry[zeroFramedAry[i][j]] = min;
              zeroFramedAry[i][j] = min; 
            }
          }
          zeroFramedAry[i][j] = EQAry[zeroFramedAry[i][j]];
        }
      }
    }
  }
 
  void connectPass3() {
    for (int i=1; i<numRows+2; i++) {
      CCProperty[i-1].minR = numRows;
      CCProperty[i-1].minC = numCols;
      for (int j=1; j<numCols+2; j++) {
        if (zeroFramedAry[i][j] > 0) {
          zeroFramedAry[i][j] = EQAry[zeroFramedAry[i][j]];
          
          int index = zeroFramedAry[i][j]-1;
          CCProperty[index].label = zeroFramedAry[i][j];
          CCProperty[index].numPixels++;
 
          CCProperty[index].minR = (i < CCProperty[index].minR) ? i : CCProperty[index].minR;
          CCProperty[index].minC = (j < CCProperty[index].minC) ? j : CCProperty[index].minC;
          CCProperty[index].maxR = (i > CCProperty[index].maxR) ? i : CCProperty[index].maxR; 
          CCProperty[index].maxC = (j > CCProperty[index].maxC) ? j : CCProperty[index].maxC;
          
        }
      }
    }
  }
 
  void drawBoxes() {
    int index = 0;
 
    while (index < trueNumCC) {
      Property current_property = CCProperty[index];
      int minRow = current_property.minR++;
      int minCol = current_property.minC++;
      int maxRow = current_property.maxR++;
      int maxCol = current_property.maxC++;
      int label = current_property.label;
 
      for (int i=minCol; i<=maxCol; i++) {
        zeroFramedAry[minRow][i] = label;
        zeroFramedAry[maxRow][i] = label;
      }
      for (int i=minRow; i<=maxRow; i++) {
        zeroFramedAry[i][minCol] = label;
        zeroFramedAry[i][maxCol] = label;
      }
      index++;
    }  
  }
 
  void updateEQ(int value) {
    int index = 0;
    while (nonZeroNeighborAry[index] != 0 && index < 5) {
      EQAry[nonZeroNeighborAry[index]] = value;
      index++;
    }
  }
 
  int manageEQAry() {
    int relabel = 0;
    int index = 1;
    while(index < newLabel) {
      if (index != EQAry[index]) {
          EQAry[index] = EQAry[EQAry[index]];
      } else {
          relabel++;
          EQAry[index] = relabel;
      }
      index++;
    }
    return relabel;
  }
 
  void printCCproperty(ofstream &outFile) { 
    outFile << numRows << " " << numCols << " " << newMin << " " << newMax << endl;
    outFile << trueNumCC << endl;
    outFile << endl;
 
    for (int i=0; i<trueNumCC; i++) {
      outFile << CCProperty[i].label << endl; 
      outFile << CCProperty[i].numPixels << endl;
      outFile << CCProperty[i].minR << " " << CCProperty[i].minC << endl;
      outFile << CCProperty[i].maxR << " " << CCProperty[i].maxC << endl;
      outFile << endl;
    }
  }
 
  void printEQAry(ofstream &outFile) { 
    for (int i = 0; i< newLabel; i++) {
      outFile << "EQAry[" << i << "] = " << EQAry[i] << endl;
    }
    outFile << endl;
  }
 
  void printImg(ofstream &outFile) {
    outFile << numRows << " " << numCols << " " << newMin << " " << newMax << endl;
 
    string str = to_string(maxVal);
    int width = str.length();
    for (int i = 1; i < numRows+1; i++) {
      for (int j = 1; j < numCols+1; j++) {
        outFile << zeroFramedAry[i][j] << " ";
        str = to_string(zeroFramedAry[i][j]);
        int ww = str.length();
        while (ww <= width) {
        outFile << " ";
        ww++;
        }
      }
      outFile << endl;
    }
  }
 
};
 
int main(int argc, char *argv[]) {
  ifstream inFile(argv[1]);
  int connectness = std::atoi(argv[2]);
  ofstream rfPrettyPrintFile(argv[3]);
  ofstream labelFile(argv[4]);
  ofstream propertyFile(argv[5]);
 
  CCLabel cc(inFile);
 
  cc.zero2D();
  cc.loadImage(inFile);
 
  if (connectness == 4) {
    cc.connect4Pass1();
    rfPrettyPrintFile << "Result of pass 1 for 4 connected:" << endl;
    cc.imgReformat(rfPrettyPrintFile);
    rfPrettyPrintFile <<"New label is " << cc.newLabel-1 << " and EQAry after pass 1:" << endl;
    cc.printEQAry(rfPrettyPrintFile); 
    cc.connect4Pass2();
    rfPrettyPrintFile << "Result of pass 2 for 4 connected:" << endl;
    cc.imgReformat(rfPrettyPrintFile);
    rfPrettyPrintFile << "New label is " << cc.newLabel-1 << " and EQAry after pass 2:" << endl;
    cc.printEQAry(rfPrettyPrintFile);
  }
 
  if (connectness == 8) {
    cc.connect8Pass1();
    rfPrettyPrintFile << "Result of pass 1 for 8 connected:" << endl;
    cc.imgReformat(rfPrettyPrintFile);
    rfPrettyPrintFile <<"New label is " << cc.newLabel-1 << " and EQAry after pass 1:" << endl;
    cc.printEQAry(rfPrettyPrintFile); 
    cc.connect8Pass2();
    rfPrettyPrintFile << "Result of pass 2 for 8 connected:" << endl;
    cc.imgReformat(rfPrettyPrintFile);
    rfPrettyPrintFile << "New label is " << cc.newLabel-1 << " and EQAry after pass 2:" << endl;
    cc.printEQAry(rfPrettyPrintFile);
  }
 
  cc.trueNumCC = cc.manageEQAry();
  cc.newMin = 0;
  cc.newMax = cc.trueNumCC;
  cc.CCProperty = new CCLabel::Property[cc.trueNumCC];
  rfPrettyPrintFile << "EQAry after EQ management:" << endl;
  cc.printEQAry(rfPrettyPrintFile);
  cc.connectPass3();
  rfPrettyPrintFile << "Result of pass 3:" << endl;
  cc.imgReformat(rfPrettyPrintFile);
  cc.printEQAry(rfPrettyPrintFile);
 
  cc.printImg(labelFile);
  cc.printCCproperty(propertyFile);
  cc.drawBoxes();
  rfPrettyPrintFile << "Result of bounding box drawing:" << endl;
  cc.imgReformat(rfPrettyPrintFile);
  rfPrettyPrintFile << "There are " << cc.trueNumCC << " true connected components" << endl;
 
  inFile.close();
  rfPrettyPrintFile.close();
  labelFile.close();
  propertyFile.close();
}
