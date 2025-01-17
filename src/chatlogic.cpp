#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <tuple>
#include <vector>

#include "chatbot.h"
#include "chatlogic.h"
#include "graphedge.h"
#include "graphnode.h"

ChatLogic::ChatLogic() {
  //// STUDENT CODE
  ////

  // the creation of the chatbot was moved to the load method

  ////
  //// EOF STUDENT CODE
}

ChatLogic::~ChatLogic() {
  std::cout << "deleting chat logic" << std::endl;

  //// STUDENT CODE
  ////

  // delete chatbot instance
  // chat bot is owned by a graph node -> no deletion here
  // delete _chatBot;

  // delete all nodes -- should not be needed any more, since
  // smart pointers should take care of it automatically
  // for (auto it = std::begin(_nodes); it != std::end(_nodes); ++it)
  //{
  //    delete *it;
  //}

  // delete all edges
  // this is also not necessary because the deletion of the edges
  // now is the responsibility of the GraphNode objects
  // for (auto it = std::begin(_edges); it != std::end(_edges); ++it) {
  //  delete *it;
  //}

  ////
  //// EOF STUDENT CODE
}

template <typename T>
void ChatLogic::AddAllTokensToElement(std::string tokenID, tokenlist &tokens,
                                      T &element) {
  // find all occurences for current node
  auto token = tokens.begin();
  while (true) {
    token = std::find_if(
        token, tokens.end(),
        [&tokenID](const std::pair<std::string, std::string> &pair) {
          return pair.first == tokenID;
          ;
        });
    if (token != tokens.end()) {
      element.AddToken(token->second); // add new keyword to edge
      token++;                         // increment iterator to next element
    } else {
      break; // quit infinite while-loop
    }
  }
}

void ChatLogic::LoadAnswerGraphFromFile(std::string filename) {
  // load file with answer graph elements
  std::ifstream file(filename);

  // check for file availability and process it line by line
  if (file) {
    // loop over all lines in the file
    std::string lineStr;
    while (getline(file, lineStr)) {
      // extract all tokens from current line
      tokenlist tokens;
      while (lineStr.size() > 0) {
        // extract next token
        int posTokenFront = lineStr.find("<");
        int posTokenBack = lineStr.find(">");
        if (posTokenFront < 0 || posTokenBack < 0)
          break; // quit loop if no complete token has been found
        std::string tokenStr =
            lineStr.substr(posTokenFront + 1, posTokenBack - 1);

        // extract token type and info
        int posTokenInfo = tokenStr.find(":");
        if (posTokenInfo != std::string::npos) {
          std::string tokenType = tokenStr.substr(0, posTokenInfo);
          std::string tokenInfo =
              tokenStr.substr(posTokenInfo + 1, tokenStr.size() - 1);

          // add token to vector
          tokens.push_back(std::make_pair(tokenType, tokenInfo));
        }

        // remove token from current line
        lineStr = lineStr.substr(posTokenBack + 1, lineStr.size());
      }

      // process tokens for current line
      auto type =
          std::find_if(tokens.begin(), tokens.end(),
                       [](const std::pair<std::string, std::string> &pair) {
                         return pair.first == "TYPE";
                       });
      if (type != tokens.end()) {
        // check for id
        auto idToken =
            std::find_if(tokens.begin(), tokens.end(),
                         [](const std::pair<std::string, std::string> &pair) {
                           return pair.first == "ID";
                         });
        if (idToken != tokens.end()) {
          // extract id from token
          int id = std::stoi(idToken->second);

          // node-based processing
          if (type->second == "NODE") {
            //// STUDENT CODE
            ////

            // check if node with this ID exists already

            // I understand that (a) the basic problem with lambda predicates
            // and unique pointers is that we should not move the unique pointer
            // between scopes unless we want to transfer ownership (which we do
            // not want to do here), while (b) it is not good to pass references
            // to smart pointers around. Would it be okay to work with a
            // reference here, since this is just a predicate function and the
            // smart pointer is not passed to a different class?
            auto newNode = std::find_if(_nodes.begin(), _nodes.end(),
                                        [&id](unique_ptr<GraphNode> &nodePtr) {
                                          return nodePtr->GetID() == id;
                                        });

            // create new element if ID does not yet exist
            if (newNode == _nodes.end()) {
              _nodes.emplace_back(std::make_unique<GraphNode>(id));
              newNode = _nodes.end() - 1; // get iterator to last element

              // add all answers to current node
              AddAllTokensToElement("ANSWER", tokens, **newNode);
            }

            ////
            //// EOF STUDENT CODE
          }

          // edge-based processing
          if (type->second == "EDGE") {
            //// STUDENT CODE
            ////

            // find tokens for incoming (parent) and outgoing (child) node
            auto parentToken = std::find_if(
                tokens.begin(), tokens.end(),
                [](const std::pair<std::string, std::string> &pair) {
                  return pair.first == "PARENT";
                });
            auto childToken = std::find_if(
                tokens.begin(), tokens.end(),
                [](const std::pair<std::string, std::string> &pair) {
                  return pair.first == "CHILD";
                });

            if (parentToken != tokens.end() && childToken != tokens.end()) {
              // get iterator on incoming and outgoing node via ID search
              auto parentNode = std::find_if(
                  _nodes.begin(), _nodes.end(),
                  [&parentToken](unique_ptr<GraphNode> &node) {
                    return node->GetID() == std::stoi(parentToken->second);
                  });
              auto childNode = std::find_if(
                  _nodes.begin(), _nodes.end(),
                  [&childToken](unique_ptr<GraphNode> &node) {
                    return node->GetID() == std::stoi(childToken->second);
                  });

              // create new edge
              unique_ptr<GraphEdge> edgePtr = std::make_unique<GraphEdge>(id);
              edgePtr->SetChildNode((*childNode).get());
              edgePtr->SetParentNode((*parentNode).get());
              // Chat logic has just the handles, not the ownership ->
              // addressing edges via raw pointers
              //_edges.push_back(edgePtr.get());

              // find all keywords for current node
              AddAllTokensToElement("KEYWORD", tokens, *edgePtr);

              // store reference in child node and parent node
              (*childNode)->AddEdgeToParentNode(edgePtr.get());
              (*parentNode)->AddEdgeToChildNode(std::move(edgePtr));
            }

            ////
            //// EOF STUDENT CODE
          }
        } else {
          std::cout << "Error: ID missing. Line is ignored!" << std::endl;
        }
      }
    } // eof loop over all lines in the file

    file.close();

  } // eof check for file availability
  else {
    std::cout << "File could not be opened!" << std::endl;
    return;
  }

  //// STUDENT CODE
  ////

  // identify root node
  GraphNode *rootNode = nullptr;
  for (auto it = std::begin(_nodes); it != std::end(_nodes); ++it) {
    // search for nodes which have no incoming edges
    if ((*it)->GetNumberOfParents() == 0) {

      if (rootNode == nullptr) {
        rootNode = (*it).get(); // assign current node to root
      } else {
        std::cout << "ERROR : Multiple root nodes detected" << std::endl;
      }
    }
  }

  // Code in the previous submission:

  // chatbot created directly within manager-object
  // unique_ptr<ChatBot> chatBotUnPtr =
  //     std::make_unique<ChatBot>("../images/chatbot.png");
  // SetChatbotHandle(chatBotUnPtr.get());
  // chatBotUnPtr->SetChatLogicHandle(this);

  // // add chatbot to graph root node
  // _chatBot->SetRootNode(rootNode);
  // // transfering ownership to the root node
  // rootNode->MoveChatbotHere(std::move(chatBotUnPtr));

  // Code recommended by the reviewer:

  ChatBot chatBot = ChatBot("../images/chatbot.png");
  chatBot.SetChatLogicHandle(this);

  // add chatbot to graph root node
  chatBot.SetRootNode(rootNode);

  // recommended by reviewer, but inconsistent with the code recommended
  // for graphnode.cpp
  // rootNode->MoveChatbotHere(std::make_unique<ChatBot>(std::move(chatBot)));

  rootNode->MoveChatbotHere(std::move(chatBot));

  ////
  //// EOF STUDENT CODE
}

void ChatLogic::SetPanelDialogHandle(ChatBotPanelDialog *panelDialog) {
  _panelDialog = panelDialog;
}

void ChatLogic::SetChatbotHandle(ChatBot *chatbot) { _chatBot = chatbot; }

void ChatLogic::SendMessageToChatbot(std::string message) {
  _chatBot->ReceiveMessageFromUser(message);
}

void ChatLogic::SendMessageToUser(std::string message) {
  _panelDialog->PrintChatbotResponse(message);
}

wxBitmap *ChatLogic::GetImageFromChatbot() {
  return _chatBot->GetImageHandle();
}
