#include <iostream>
#include <fstream>
#include <locale>
#include <sstream>
#include <string>
#include <vector>

#include "load.hh"
#include "quest.hh"
#include "questList.hh"
#include "save.hh"

//Prints a list of the possible actions to take in the main menu
void help(){
  std::cout << "List of actions:\n\thelp / h: Get this list\n\texit / e: Exit this program (does not save progress)\n\tsave / s: Go to save menu\n\tstory / st: Go to story point menu\n\tquest / q: Go to quest menu\n\tprint / p: Go to print menu" << std::endl;
}

int checkStoryPointAction(std::string action){
  if(action == "1"){
    return 1;
  } else if (action == "2"){
    return 2;
  } else if (action == "3"){
    return 3;
  }
  return -1;
}

int checkStoryPointChoice(std::string answer, int numStoryPoints){
  int answerNum = 0;
  try{
    answerNum = std::stoi(answer);
  } catch(std::exception& e){
    std::cout << "Invalid answer." << std::endl;
    return -1;
  }
  if(answerNum >= numStoryPoints){
    std::cout << "Invalid answer." << std::endl;
    return -1;
  }
  return answerNum;
}

void storyPointMenu(QuestList* quests){
  bool done = false;
  while(!done){
    std::cout << "Would you like to pass a story point (1), bring back a story point (2), or exit this menu (3)?" << std::endl;
    std::string answer;
    std::cin >> answer;
    int actionNum = checkStoryPointAction(answer);
    if(actionNum == -1){
      std::cout << "Invalid response." << std::endl;
    } else if (actionNum == 3){
      done = !done;
    } else if (actionNum == 2){
      std::cout << "Which story point do you want to bring back? (Enter the number of the desired point)\nAny story point after the chosen point will also be passed." << std::endl;
      quests->printStoryPointsGarbage();
      std::cin >> answer;
      int storyPointGarbageSize = quests->getStoryPointGarbageSize();
      int answerNum = checkStoryPointChoice(answer, storyPointGarbageSize);
      if(answerNum != -1){
        quests->reuseStoryPoint(answerNum);
      }
    } else {
      std::cout << "Which story point do you want to pass? (Enter the number of the desired point)\nAny story point before the chosen point will also be passed." << std::endl;
      quests->printStoryPoints();
      std::cin >> answer;
      int numStoryPoints = quests->getStoryPointsSize();
      int answerNum = checkStoryPointChoice(answer, numStoryPoints);
      if(answerNum != -1){
        quests->removeStoryPoint(answerNum);
      }
    }
    if(!done){
      std::cout << "Are you done moving story points? (yes/no)" << std::endl;
      std::cin >> answer;
      if(answer == "yes"){
        done = !done;
      } else if (answer != "no"){
        std::cout << "Invalid answer." << std::endl;
      }
    }
  }
}

void questMenu(QuestList* quests){
  //TODO
}

//Asks the user what to print
void printMenu(QuestList* quests){
  bool donePrinting = false;
  while(!donePrinting){
    std::cout << "Would you like to print all quest information (1) or only one information category (2)?" << std::endl;
    std::string answer;
    std::cin >> answer;
    if(answer == "1"){
      quests->printOpen();
    } else if(answer == "2"){
      std::cout << "What category would you like to print by?" << std::endl;
      std::cin >> answer;
      quests->printCategory(answer);
    } else {
      std::cout << "That is not an accepted input." << std::endl;
    }
    std::cout << "Would you like to return to the main menu? (yes/no)" << std::endl;
    std::cin >> answer;
    if(answer == "yes"){
      donePrinting = !donePrinting;
    }
  }
}

//Executes the action desired from the main menu
bool takeAction(int code, QuestList* quests){
  switch(code){
    case -1:
      std::cout << "Invalid action. Use help or h for a list of actions." << std::endl;
      break;
    case 1:
      help();
      break;
    case 2:
      return true;
    case 3:
      Save::saveMenu(quests);
      break;
    case 4:
      storyPointMenu(quests);
      break;
    case 5:
      questMenu(quests);
      break;
    case 6:
      printMenu(quests);
      break;
    default:
      std::cout << "Error: Invalid action code given." << std::endl;
      break;
  }
  return false;
}

//Checks to see if a given action at the main menu associates with a possible action (not case sensitive). Returns -1 if none are found.
//All codes should be self-explainable
int checkActions(std::string caseAction){
  std::locale loc;
  std::string action;
  for(std::string::size_type i = 0; i < caseAction.length(); i++){
    action += std::tolower(caseAction[i], loc);
  }
  if(action == "help" || action == "h"){
    return 1;
  }
  if(action == "exit" || action == "e"){
    return 2;
  }
  if(action == "save" || action == "s"){
    return 3;
  }
  if(action == "story" || action == "st"){
    return 4;
  }
  if(action == "quest" || action == "q"){
    return 5;
  }
  if(action == "print" || action == "p"){
    return 6;
  }
  return -1;
}

int main(int argc, char** argv){
  if(argc != 2){
    std::cout << "Usage: ./main [Base .txt file / Saved .txt file]" << std::endl;
    exit(0);
  }
  //Gets user input on the type of file used
  std::cout << "Is this file a Base or a Save? (1 or 2)" << std::endl;
  bool validAnswer = false;
  std::string answer;
  while(!validAnswer){
    std::cin >> answer;
    if(answer == "1" || answer == "2"){
      validAnswer = true;
    } else {
      std::cout << "Please enter either 1 (Base) or 2 (Save)." << std::endl;
    }
  }
  //Loads the file and uses the main menu to get user input and execute that input.
  QuestList* quests = Load::getQuestList(argv[1], answer);
  bool wantExit = false;
  while(!wantExit){
    std::cout << "Enter what action you would like to take (type 'help' or 'h' for help)" << std::endl;
    std::cin >> answer;
    int actionCode = checkActions(answer);
    wantExit = takeAction(actionCode, quests);
  }
  std::vector<Quest*> questList = quests->getQuests();
  std::vector<Quest*> garbageList = quests->getQuestGarbage();
  for(int i = 0; i < questList.size(); i++){
    delete questList[i];
  }
  for(int i = 0; i < garbageList.size(); i++){
    delete garbageList[i];
  }
  delete quests;
}
