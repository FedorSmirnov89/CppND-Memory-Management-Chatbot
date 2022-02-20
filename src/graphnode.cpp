#include "graphnode.h"
#include "graphedge.h"

#include <iostream>

GraphNode::GraphNode(int id) {
  _id = id;
}

GraphNode::~GraphNode() {
  //// STUDENT CODE
  ////
  std::cout << "deleting graph node " << GetID() << std::endl;

  // chat bot destroyed if it is in the current node

  // should not be necessary since handled by the unique pointer
  // if (_chatBot != nullptr){
  //    std::cout << "chatbot was in node " << GetID()  << std::endl;
  //    delete _chatBot;
  //}

  //     delete _chatBot;

  ////
  //// EOF STUDENT CODE
}

void GraphNode::AddToken(std::string token) { _answers.push_back(token); }

void GraphNode::AddEdgeToParentNode(GraphEdge *edge) {
  _parentEdges.push_back(edge);
}

void GraphNode::AddEdgeToChildNode(unique_ptr<GraphEdge> edgePtr) {
  _childEdges.push_back(std::move(edgePtr));
}

//// STUDENT CODE
////
void GraphNode::MoveChatbotHere(ChatBot chatBot) {
  std::cout << "Moving Chatbot here -- node " << GetID() << std::endl;
  _chatBot = std::move(chatBot);
  _chatBot.SetCurrentNode(this);
}

void GraphNode::MoveChatbotToNewNode(GraphNode *newNode) {
  std::cout << "Moving Chatbot" << std::endl;
  newNode->MoveChatbotHere(std::move(_chatBot));
  //_chatBot = nullptr; // invalidate pointer at source
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