#include "graphnode.h"
#include "graphedge.h"

#include <iostream>

GraphNode::GraphNode(int id) { _id = id; _chatBot = nullptr;}

GraphNode::~GraphNode() {
  //// STUDENT CODE
  ////
  std::cout << "deleting graph node " << GetID() << std::endl;

  // chat bot destroyed if it is in the current node
  if (_chatBot != nullptr){
      std::cout << "chatbot was in node " << GetID()  << std::endl;
      delete _chatBot;
  }

  //     delete _chatBot;

  ////
  //// EOF STUDENT CODE
}

void GraphNode::AddToken(std::string token) { _answers.push_back(token); }

void GraphNode::AddEdgeToParentNode(GraphEdge *edge) {
  _parentEdges.push_back(edge);
}

void GraphNode::AddEdgeToChildNode(GraphEdge&& edge) {
  _childEdges.push_back(std::make_unique<GraphEdge>(edge));
}

//// STUDENT CODE
////
void GraphNode::MoveChatbotHere(ChatBot *chatbot) {
  std::cout << "Moving Chatbot here -- node " << GetID() << std::endl;
  _chatBot = chatbot;
  _chatBot->SetCurrentNode(this);
}

void GraphNode::MoveChatbotToNewNode(GraphNode *newNode) {
  std::cout << "Moving Chatbot" << std::endl;
  newNode->MoveChatbotHere(_chatBot);
  _chatBot = nullptr; // invalidate pointer at source
}
////
//// EOF STUDENT CODE

GraphEdge *GraphNode::GetChildEdgeAtIndex(int index) {
  //// STUDENT CODE
  ////

  // returning raw pointer made from the unique pointer 
  // at the requested index
  return _childEdges[index].get();

  ////
  //// EOF STUDENT CODE
}