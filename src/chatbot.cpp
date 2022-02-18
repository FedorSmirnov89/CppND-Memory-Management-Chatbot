#include <algorithm>
#include <ctime>
#include <iostream>
#include <random>

#include "chatbot.h"
#include "chatlogic.h"
#include "graphedge.h"
#include "graphnode.h"

int ChatBot::referenceCounter;

// constructor WITHOUT memory allocation
ChatBot::ChatBot() {

  std::cout << "ChatBot Constructor invalidate" << std::endl;
  // invalidate data handles
  _image = nullptr;
  _chatLogic = nullptr;
  _rootNode = nullptr;
}

// constructor WITH memory allocation
ChatBot::ChatBot(std::string filename) {
  std::cout << "ChatBot Constructor" << std::endl;

  // invalidate data handles
  _chatLogic = nullptr;
  _rootNode = nullptr;

  // load image into heap memory
  _image = new wxBitmap(filename, wxBITMAP_TYPE_PNG);

  referenceCounter = 1;
}

ChatBot::~ChatBot() {
  std::cout << "ChatBot Destructor" << std::endl;

  if (--referenceCounter == 0) {
    // deallocate heap memory
    if (_image != NULL) // Attention: wxWidgets used NULL and not nullptr
    {
      delete _image;
      _image = NULL;
    }
  }
}

//// STUDENT CODE
////

// Copy constructor
ChatBot::ChatBot(ChatBot &other) {
  // I am not sure why it would be necessary to copy the chatbot, but in general
  // I would assume that in situations with multiple chatbot instances, we would
  // want them to all have the handles to the same objects. To make sure that
  // the owned image resource is properly deallocated, we are keeping a
  // reference counter (the cleaner implementation would be a shared pointer).

  std::cout << "Copying the chat bot using the constructor." << std::endl;

  // not owned handles are copied
  _chatLogic = other._chatLogic;
  _rootNode = other._rootNode;

  // the owned class is also just copied while updating the reference counter
  _image = other._image;
  referenceCounter++;
}

// Copy assignment operator
ChatBot &ChatBot::operator=(ChatBot &other) {
  // Same reasoning and behavior as for the copy constructor

  std::cout << "Copying the chat bot using the copy assignment operator."
            << std::endl;

  if (&other != this) {
    // copying the not owned handles
    _chatLogic = other._chatLogic;
    _rootNode = other._rootNode;

    // it should only be necessary to copy the image if it is a nullptr
    // (since all chat bot instances point to the same image)
    if (_image == nullptr) {
      referenceCounter++;
      _image = other._image;
    }
  }
  return *this;
}

// Move constructor
ChatBot::ChatBot(ChatBot &&other) {
  // here, we want to copy the handles which are not owned
  // since we are using a shared ownership, we can just copy the image handle
  // while incrementing the reference pointer without running into any
  // deallocation problems (we could invalidate the handle of 'other' by setting
  // it to NULL, but I don't really see the point of doing it)

  std::cout << "Moving the chat bot using the constructor." << std::endl;

  _chatLogic = other._chatLogic;
  _rootNode = other._rootNode;

  _image = other._image;
  referenceCounter++;
}

// Move assignment operator
ChatBot &ChatBot::operator=(ChatBot &&other) {
  // same logic as for the move constructor
  std::cout << "Moving the chat bot using the move assignment operator."
            << std::endl;

  if (&other != this) {

    _chatLogic = other._chatLogic;
    _rootNode = other._rootNode;

    _image = other._image;
    referenceCounter++;
  }
  return *this;
}

////
//// EOF STUDENT CODE

void ChatBot::ReceiveMessageFromUser(std::string message) {
  // loop over all edges and keywords and compute Levenshtein distance to query
  typedef std::pair<GraphEdge *, int> EdgeDist;
  std::vector<EdgeDist> levDists; // format is <ptr,levDist>

  for (size_t i = 0; i < _currentNode->GetNumberOfChildEdges(); ++i) {
    GraphEdge *edge = _currentNode->GetChildEdgeAtIndex(i);
    for (auto keyword : edge->GetKeywords()) {
      EdgeDist ed{edge, ComputeLevenshteinDistance(keyword, message)};
      levDists.push_back(ed);
    }
  }

  // select best fitting edge to proceed along
  GraphNode *newNode;
  if (levDists.size() > 0) {
    // sort in ascending order of Levenshtein distance (best fit is at the top)
    std::sort(levDists.begin(), levDists.end(),
              [](const EdgeDist &a, const EdgeDist &b) {
                return a.second < b.second;
              });
    newNode = levDists.at(0).first->GetChildNode(); // after sorting the best
                                                    // edge is at first position
  } else {
    // go back to root node
    newNode = _rootNode;
  }

  // tell current node to move chatbot to new node
  _currentNode->MoveChatbotToNewNode(newNode);
}

void ChatBot::SetCurrentNode(GraphNode *node) {
  // update pointer to current node
  _currentNode = node;

  // select a random node answer (if several answers should exist)
  std::vector<std::string> answers = _currentNode->GetAnswers();
  std::mt19937 generator(int(std::time(0)));
  std::uniform_int_distribution<int> dis(0, answers.size() - 1);
  std::string answer = answers.at(dis(generator));

  // send selected node answer to user
  _chatLogic->SendMessageToUser(answer);
}

int ChatBot::ComputeLevenshteinDistance(std::string s1, std::string s2) {
  // convert both strings to upper-case before comparing
  std::transform(s1.begin(), s1.end(), s1.begin(), ::toupper);
  std::transform(s2.begin(), s2.end(), s2.begin(), ::toupper);

  // compute Levenshtein distance measure between both strings
  const size_t m(s1.size());
  const size_t n(s2.size());

  if (m == 0)
    return n;
  if (n == 0)
    return m;

  size_t *costs = new size_t[n + 1];

  for (size_t k = 0; k <= n; k++)
    costs[k] = k;

  size_t i = 0;
  for (std::string::const_iterator it1 = s1.begin(); it1 != s1.end();
       ++it1, ++i) {
    costs[0] = i + 1;
    size_t corner = i;

    size_t j = 0;
    for (std::string::const_iterator it2 = s2.begin(); it2 != s2.end();
         ++it2, ++j) {
      size_t upper = costs[j + 1];
      if (*it1 == *it2) {
        costs[j + 1] = corner;
      } else {
        size_t t(upper < corner ? upper : corner);
        costs[j + 1] = (costs[j] < t ? costs[j] : t) + 1;
      }

      corner = upper;
    }
  }

  int result = costs[n];
  delete[] costs;

  return result;
}