#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <list>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <queue>
#include <bitset>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <sstream>

using namespace std;

//Class to store a vertex and its corresponding key, which is used in Prim's using Fibonacci Heap
class VertexKey
{
  public:
   int vertex;
   int key;

  VertexKey(int vertex, int key)
   {
     this->vertex = vertex;
     this->key = key;
   }
};

//Class For a Fibonacci Heap Node
class FibonacciHeapNode
{
   public:

     int degree;
     FibonacciHeapNode *child;
     VertexKey *data;                       //Vertex and its Key
     FibonacciHeapNode *right;
     FibonacciHeapNode *left;
     FibonacciHeapNode *parent;
     bool childCut;

     FibonacciHeapNode(VertexKey *data)
       {
          if(data == NULL)
            {
              cout<<"Data Argument is Null. Exiting..";
              exit(1);
            }
          this->data = data;
          right=left=parent=child=NULL;
          degree=0;
          childCut=false;
       }

     //Prints the node
     void print()
       {
          cout<<endl;
          cout<<"Address: "<<this<<endl;
          cout<<"Degree: "<<degree<<endl;
          cout<<"Vertex: "<<data->vertex<<" Key: "<<data->key<<endl;
          cout<<"ChildCut: "<<childCut<<endl;
          cout<<"Right: "<<right<<endl;
          cout<<"Left: "<<left<<endl;
          cout<<"Parent: "<<parent<<endl;
          cout<<"Child: "<<child<<endl;
          cout<<endl;
       }
};

//Fibonacci Heap
class FibonacciHeap
{
   public:

     int numberOfNodes;
     FibonacciHeapNode *min;                //Points minimum node in heap

     FibonacciHeap()
       {
         min = NULL;
         numberOfNodes = 0;
       }

     void insert(FibonacciHeapNode *node)   //Insert a new node
       {
          if(min == NULL)                   //If null, make it min
            {
              min = node;
              min->right=min->left=min;
            }
          else                              //Else add it and update min if needed
            {
              node->right = min->right;
              node->left = min;
              min->right->left = node;
              min->right = node;
              if(node->data->key < min->data->key)
                 min = node;
            }
       }

     void print()                           //Print all nodes in Heap
       {
          if(min == NULL)
             {
                cout<<"heap is empty "<<endl;
                return;
             }

          cout<<endl<<"Min is: "<<min<<endl;

          queue<FibonacciHeapNode*> bfsQ;
          queue<FibonacciHeapNode*> bfsQForSiblings;
          bfsQ.push(min);

          int i=0, j=0;

          while(!bfsQ.empty())
            {
               FibonacciHeapNode *visit = bfsQ.front();
               bfsQ.pop();
               visit->print();

               //queuing siblings
               FibonacciHeapNode *itr = visit->right;
               while(itr != visit)
                 {
                    bfsQForSiblings.push(itr);
                    itr = itr->right;
                 }

               //queuing children
               if(visit->child != NULL)
                   bfsQ.push(visit->child);

               while(!bfsQForSiblings.empty())
                 {
                    FibonacciHeapNode *tmp = bfsQForSiblings.front();
                    bfsQForSiblings.pop();
                    tmp->print();
                    if(tmp->child != NULL)
                       bfsQ.push(tmp->child);
                 }
            }

       }

     //returns pointer to minimum key node in list and sets parent as NULL and childcut as false in all nodes in list if specified.
     FibonacciHeapNode* minKeyNode(FibonacciHeapNode *list, bool setParentAsNull, bool setChildCutAsFalse)
       {
          FibonacciHeapNode *listitr = list->right;
          int minKey = list->data->key;
          FibonacciHeapNode *minPointer = list;

          if(setParentAsNull == true)
            list->parent=NULL;
          if(setChildCutAsFalse == true)
            list->childCut=false;

          while(listitr != list)
            {
              if(listitr->data->key < minKey)
                {
                   minKey = listitr->data->key;
                   minPointer = listitr;
                }
              if(setParentAsNull == true)
                 listitr->parent = NULL;
              if(setChildCutAsFalse == true)
                 listitr->childCut = false;
              listitr = listitr->right;
            }
          return minPointer;
       }

     //Combines a list pointed by list with the top level list
     void combineListWithTopLevelList(FibonacciHeapNode *list)
       {
          if(list == NULL)
            {
              return;
            }

          //Find minimum key node in list
          FibonacciHeapNode *minPointer = minKeyNode(list, true, false);

          //Merge list with top level list
          list->left->right = min->right;
          min->right->left = list->left;
          min->right = list;
          list->left = min;

          //Update min pointer
          if(minPointer->data->key < min->data->key)
            {
              min = minPointer;
            }
       }

     //Removes minimum node and returns corresponding vertex and min key
     VertexKey* removeMin()
      {
        //no min
        if(min == NULL)
          return NULL;

        //Only one top level root
        if(min->right==min && min->left==min)
          {
             if(min->child == NULL)         //has no child
              {
                 VertexKey* data = min->data;
                 min = NULL;
                 return data;
              }
             else                           //has childs
              {
                 FibonacciHeapNode *minPointer = minKeyNode(min->child, true, false);
                 VertexKey *data = min->data;
                 min = minPointer;
                 pairwiseCombine();
                 return data;
              }
          }
        else                                //More than one top level roots
          {

            //remove min from top level list
            min->left->right = min->right;
            min->right->left = min->left;

            FibonacciHeapNode *minTemp = min;

            //Find new min of remaining siblings of min
            min = minKeyNode(minTemp->right, false, false);

            //Combine childrens to top level and update min
            combineListWithTopLevelList(minTemp->child);

            //Perform a pairwise combine
            pairwiseCombine();

            return minTemp->data;
          }
      }

      //Pairwise Combine the top level list roots with equal degrees
      void pairwiseCombine()
        {
           vector<FibonacciHeapNode*> treeOfDegree;
           FibonacciHeapNode *itr = min;

           //Store pointers to each trees
           vector<FibonacciHeapNode*> allTrees;
           allTrees.push_back(itr);
           itr = itr->right;
           while(itr != min)
             {
                allTrees.push_back(itr);
                itr = itr->right;
             }

           //Iterate through each tree
           vector<FibonacciHeapNode*>::iterator startItr = allTrees.begin();
           vector<FibonacciHeapNode*>::iterator endItr = allTrees.end();
           while(startItr != endItr)
             {
                FibonacciHeapNode * linkedPtr = (*startItr);
                int degree =  linkedPtr->degree;

                //Run until current tree's complete combine is completed
                while(true)
                  {
                    //Making Vector for storing per degree tree big enough to accomodate max degree
                    while(degree >= treeOfDegree.size() )
                         treeOfDegree.push_back(NULL);

                    if(treeOfDegree[degree] == NULL)
                      {
                        treeOfDegree[degree] = linkedPtr;
                        break;
                      }
                    else
                      {
                        linkedPtr = linkRoots(linkedPtr, treeOfDegree[degree]);
                        treeOfDegree[degree] = NULL;
                        degree = degree + 1;
                      }
                  }

                startItr++;
             }

           //Create a root level list with updated min pointer from combined trees
           min = NULL;
           int minKey;
           startItr = treeOfDegree.begin();
           endItr = treeOfDegree.end();
           while(startItr != endItr)
             {
                //If certain degree exist, add it to list
                if( (*startItr) != NULL)
                  {
                    if(min == NULL)         //If minimum is null, make this node minimum
                      {
                         min = (*startItr);
                         min->left = min->right = min;
                         minKey = (*startItr)->data->key;
                      }
                    else                    //If min exist
                      {
                         //Add the tree to list
                         min->right->left = (*startItr);
                         (*startItr)->right = min->right;
                         min->right = (*startItr);
                         (*startItr)->left = min;

                         //Update min pointer
                         if( (*startItr)->data->key < minKey )
                            {
                               minKey = (*startItr)->data->key;
                               min = (*startItr);
                            }
                      }
                  }
                startItr++;
             }
        }

      //Removes an arbitary node pointed by nodeToRemove and returns corresponding vertex and key
      VertexKey* removeArbitary(FibonacciHeapNode* nodeToRemove)
        {
          if(nodeToRemove == NULL)
            {
              cout<<"Cannot Remove NULL: Exiting..";
              exit(1);
            }

          VertexKey *data = nodeToRemove->data;
          bool firstNodeDeleted = false;

          do {
               //remove node from list
               nodeToRemove->left->right = nodeToRemove->right;
               nodeToRemove->right->left = nodeToRemove->left;

               //if parent points to this child, update parent's child pointer
               if(nodeToRemove->parent->child = nodeToRemove)
                 {
                                  //If there are no siblings, make parent->child as null
                   if(nodeToRemove->right==nodeToRemove && nodeToRemove->left==nodeToRemove)
                     {
                       nodeToRemove->parent->child = NULL;
                     }
                   else           //If has siblings, point parent's child to removed node's sibling
                     {
                       nodeToRemove->parent->child = nodeToRemove->right;
                     }
                 }

               FibonacciHeapNode *temp;

               if(firstNodeDeleted == false)                    //Combine only childrens to top level list
                 {
                    FibonacciHeapNode *temp = nodeToRemove->parent;
                    combineListWithTopLevelList(nodeToRemove->child);
                    firstNodeDeleted = true;
                 }
               else
                 {                                              //Combine the node algonwith childrens to top level list
                    nodeToRemove->right = nodeToRemove;
                    nodeToRemove->left = nodeToRemove;
                    temp = nodeToRemove->parent;
                    combineListWithTopLevelList(nodeToRemove);
                 }

               nodeToRemove = temp;

               if(nodeToRemove != NULL)
                  nodeToRemove->degree = nodeToRemove->degree-1;

              } while(nodeToRemove != NULL && nodeToRemove->childCut == true);

           if(nodeToRemove != NULL)
              nodeToRemove->childCut = true;

           return data;
        }

      //Decrease Key of a node pointed by nodeToDecreaseKey to a value pointed by new key
      void decreaseKey(FibonacciHeapNode *nodeToDecreaseKey, int newKey)
        {

           if(nodeToDecreaseKey == NULL)
             {
               cout<<"Argument is null for descrease key: ..Exiting ";
               exit(0);
             }

           //If new key is not smaller than existing key, return
           if(nodeToDecreaseKey->data->key < newKey)
             {
               cout<<"Already smaller key present: "<<endl;
               return;
             }

           //Descrease key
           nodeToDecreaseKey->data->key = newKey;

           //If no parent exist than no min tree property violation and return.
           if(nodeToDecreaseKey->parent == NULL)
             {
                if(min->data->key > nodeToDecreaseKey->data->key)
                  min = nodeToDecreaseKey;
                return;
             }

           //If parent key is still smaller than decreased child than no min tree property violation and return.
           if( newKey >= nodeToDecreaseKey->parent->data->key )
              return;

           FibonacciHeapNode *nodeToRemove = nodeToDecreaseKey;

           //New key is smaller than its parent's and min tree violation. Add the node to top level list and do cascading cut
           do{
               //Remove node from list
               nodeToRemove->left->right = nodeToRemove->right;
               nodeToRemove->right->left = nodeToRemove->left;

               //If parent points to this child, update the parent's child pointer
               if(nodeToRemove->parent->child = nodeToRemove)
                 {
                   //If the node has no siblings, make parent->child as null
                   if(nodeToRemove->right==nodeToRemove && nodeToRemove->left==nodeToRemove)
                     {
                        nodeToRemove->parent->child = NULL;
                     }
                   else //If has siblings make parent point to sibling
                     {
                        nodeToRemove->parent->child = nodeToRemove->right;
                     }
                 }

               //Merge the node itself to top level list
               nodeToRemove->right = nodeToRemove;
               nodeToRemove->left = nodeToRemove;
               FibonacciHeapNode *temp = nodeToRemove->parent;
               combineListWithTopLevelList(nodeToRemove);

               nodeToRemove = temp;
               nodeToRemove->degree -=  1;

              } while(nodeToRemove->parent != NULL && nodeToRemove->childCut == true );

           if(nodeToRemove != NULL)
              nodeToRemove->childCut = true;
        }

      //Make one root child of another root. Larger root becomes child of smaller root. Pointer to smaller root is returned.
      FibonacciHeapNode* linkRoots(FibonacciHeapNode *root1, FibonacciHeapNode *root2)
        {
           //Makes root1 to have smaller root and root2 to have larger root
           if(root2->data->key < root1->data->key)
             {
                FibonacciHeapNode *temp = root1;
                root1 = root2;
                root2 = temp;
             }

           //If root1 has childs, add root2 to siblings list of root1's children
           if(root1->child != NULL)
             {
               root2->right = root1->child->right;
               root1->child->right->left = root2;
               root1->child->right = root2;
               root2->left = root1->child;
               root2->parent = root1;
               root2->childCut = false;
             }
           else                        //Just make root2 child of root1
             {
               root1->child = root2;
               root2->parent = root1;
               root2->childCut = false;
               root2->right = root2;
               root2->left = root2;
             }

           root1->degree += 1;
           return root1;
        }

};

//Node structure to store edges in adjacency list of a vertex
class EdgeNode
{
  public:
    int sourceVertex;               //Vertex whose adjacency list contains this node
    int destinationVertex;          //Vertex who is connected to source vertex
    int weight;                     //Weight of the edge from source to destination

    EdgeNode(int sourceVertex, int destinationVertex, int weight)
      {
        this->sourceVertex = sourceVertex;
        this->destinationVertex = destinationVertex;
        this->weight = weight;
      }

    bool operator== (EdgeNode &rhs)       //Returns true if two edge nodes are same
      {
        return( ( this->sourceVertex == rhs.sourceVertex ) && ( this->destinationVertex == rhs.destinationVertex ) && ( this->weight == rhs.weight ) );
      }
};

//Graph of containing vertices, their connectivity with weights of edges.
class Graph
{

  public:
    int numberOfVertices;
    int numberOfEdges;
    vector<list<EdgeNode *> *> *adjacencyList;
    bool **edgePresentArray;

  public:
    Graph(int numberOfVertices)
     {
       this->numberOfVertices = numberOfVertices;
       adjacencyList = new vector<list<EdgeNode *> *>;

       for(int i=0; i<numberOfVertices; i++)
         {
           adjacencyList->push_back(NULL);
         }

       edgePresentArray = new bool* [numberOfVertices];
       for(int i=0;i<numberOfVertices;i++)
         edgePresentArray[i] = new bool[numberOfVertices];
     }

    //Re-Allocate graph
    void reAllocateListWithNewNumberOfVertices(int numberOfVertices)
     {
       if(this->numberOfVertices > 0)
         {
          //delete previous adjacency list
          for(int i=0; i< this->adjacencyList->size(); i++)
            {
              if( (*adjacencyList)[i] != NULL )
                {
                  list<EdgeNode*>::iterator strit = (*adjacencyList)[i]->begin();
                  list<EdgeNode*>::iterator endit = (*adjacencyList)[i]->end();
                  while(strit != endit)
                    {
                       if( (*strit) != NULL)
                         delete (*strit);
                       strit++;
                    }
                 delete (*adjacencyList)[i];
                }
            }
          if(adjacencyList != NULL)
          delete adjacencyList;
         }
       this->numberOfVertices = numberOfVertices;

       //Reallocate adjacency list
       adjacencyList = new vector<list<EdgeNode *> *>;
       for(int i=0; i<numberOfVertices; i++)
         {
           adjacencyList->push_back(NULL);
         }


       edgePresentArray = new bool* [numberOfVertices];
       for(int i=0;i<numberOfVertices;i++)
         edgePresentArray[i] = new bool[numberOfVertices];
     }

    void generateRandomGraph(int density)
     {
        srand(time(NULL));

        //Total edges possible
        double totalEdgesPossible = ( ( numberOfVertices * (numberOfVertices-1 ) ) / (2.00) );
        //cout<<"Total Edges possible: "<<totalEdgesPossible<<endl;

        //Number of edges to Add based on density
        double numberOfEdgesToAdd = (density/100.00) * totalEdgesPossible;
        //cout<<"Number Of Edges To Add: "<<numberOfEdgesToAdd<<endl;

        double i = 0 ;
        double tryWithNoChanges = 0;
        double lastFound = false;

        while(i< numberOfEdgesToAdd)
          {
             //Generate random edge and cost.
             int startVertex = rand() % numberOfVertices;
             int endVertex = rand() % numberOfVertices;
             int cost = (rand() % 1000) + 1;

             bool present = edgePresentArray[startVertex][endVertex] && edgePresentArray[endVertex][startVertex];
             if((startVertex != endVertex) && !present)
                {
                  addEdge(startVertex, endVertex, cost);
                  i++;
                }

             if(!lastFound && (present || startVertex==endVertex) )
               tryWithNoChanges++;

             if(lastFound && (present || startVertex==endVertex) )
               tryWithNoChanges=1;

             //If no edge add since 1000 continuous tries
             if(tryWithNoChanges == 1000)
               {
                 //cout<<"Percentage Completed: "<< (i/numberOfEdgesToAdd)*100.00<<endl;
                 break;
               }

             lastFound = !present && startVertex!=endVertex;
          }

        //cout<<"Number Of Edges Added: "<<i<<endl;

     }

    //Adds edge for the input startvertex, endvertex and cose and returns true on success
    bool addEdge(int startVertex, int endVertex, int cost)
       {
         edgePresentArray[startVertex][endVertex] = edgePresentArray[endVertex][startVertex] = true;
         //cout<<startVertex<<" "<<endVertex<<endl;

         //Create edge node for adjacency list of start vertex and end vertex each
         EdgeNode *forStartVertexList = new EdgeNode(startVertex, endVertex, cost);
         EdgeNode *forEndVertexList = new EdgeNode(endVertex, startVertex, cost);

         //If adjacency list for start vertex does not exist, add it and add edge
         if( (*adjacencyList)[startVertex] == NULL )
            {
               list<EdgeNode *> *newList = new list<EdgeNode *>();
               (*adjacencyList)[startVertex] = newList;
               (*adjacencyList)[startVertex]->push_back(forStartVertexList);
            }
         else
            (*adjacencyList)[startVertex]->push_back(forStartVertexList);

         //If adjacency list for start vertex does not exist, add it and add edge
         if( (*adjacencyList)[endVertex] == NULL )
            {
               list<EdgeNode *> *newList = new list<EdgeNode *>();
               (*adjacencyList)[endVertex] = newList;
               (*adjacencyList)[endVertex]->push_back(forEndVertexList);
            }
         else
            (*adjacencyList)[endVertex]->push_back(forEndVertexList);

         numberOfEdges++;
         return true;
       }

    //Returns true if edge is present in adjacency list of start and end vertex
    bool edgePresent(int start, int end)
       {
         if( (*adjacencyList)[start] != NULL && (*adjacencyList)[end] != NULL)
           {
             //Finds in start vertex list
             bool result1 = false;
             list<EdgeNode *>::iterator startItr1 = (*adjacencyList)[start]->begin();
             list<EdgeNode *>::iterator endItr1 = (*adjacencyList)[start]->end();
             while( startItr1 != endItr1)
               {
                  if( (*startItr1)->destinationVertex == end )
                    result1 = true;
                  startItr1++;
               }

             //Finds in start vertex list
             bool result2 = false;
             list<EdgeNode *>::iterator startItr2 = (*adjacencyList)[end]->begin();
             list<EdgeNode *>::iterator endItr2 = (*adjacencyList)[end]->end();
             while(startItr2 != endItr2)
               {
                  if( (*startItr2)->destinationVertex == start)
                    result2 = true;
                  startItr2++;
               }

             return (result1 && result2);
           }
         else
           {
              return false;
           }
       }

    //Returns true if graph is connected by performing Breadth First Search on graph.
    bool graphConnectedOrNot()
       {
          queue<int> verticesQ;
          bool verticesVisited[numberOfVertices];
          for(int i=0; i<numberOfVertices; i++)
            verticesVisited[i] = false;

          //Choose random start vertex
          srand(time(NULL));
          int sourceVertex = rand() % numberOfVertices;

          while((*adjacencyList)[sourceVertex] == NULL)
               sourceVertex = rand() % numberOfVertices;

          verticesQ.push(sourceVertex);
          verticesVisited[sourceVertex] = true;

          //Perform Breadth First Search
          while(!verticesQ.empty())
            {
                int currentV = verticesQ.front();
                verticesQ.pop();
                list<EdgeNode*>::iterator startitr = (*adjacencyList)[currentV]->begin();
                list<EdgeNode*>::iterator enditr = (*adjacencyList)[currentV]->end();
                while(startitr != enditr)
                  {
                     if (! verticesVisited[(*startitr)->destinationVertex] )
                       {
                           verticesVisited[(*startitr)->destinationVertex] = true;
                           verticesQ.push((*startitr)->destinationVertex);
                       }
                     startitr++;
                  }
            }

          //If any node is unconnected return false
          for(int i=0; i<numberOfVertices; i++)
            if(verticesVisited[i] == false)
              return false;

          return true;
       }

     //Generate a minimum Spanning Tree by Prims algorithm using a simple Array
     void getMinimumSpanningTreeUsingPrimsSimpleScheme(string filepath)
       {
          int key[numberOfVertices];               //Stores the current minimum edge weight to each vertex
          int parent[numberOfVertices];            //Stores the vertex from which the minimum edge to each vertex exists
          bool vertexAdded[numberOfVertices];      //Stroes if the vertex is added to spanning tree yet
          list<int> currentTree;

          for(int i=0; i<numberOfVertices; i++)
            {
               key[i]=2000;
               parent[i]=-1;
               vertexAdded[i]= false;
            }

          key[0]=0;

          while(currentTree.size() < numberOfVertices)
            {
               int minVertex= -1;
               int minKey= 3000;

               //Remove min for Prim's by tracersing the entire array
               for(int i=0; i<numberOfVertices; i++)
                 {
                    if( vertexAdded[i]==false && key[i]<minKey )
                      {
                         minKey=key[i];
                         minVertex = i;
                      }
                 }
               vertexAdded[minVertex] = true;
               currentTree.push_back(minVertex);

               //Relax the all adjacent nodes to minimum node
               list<EdgeNode*>::iterator startItr = (*adjacencyList)[minVertex]->begin();
               list<EdgeNode*>::iterator endItr = (*adjacencyList)[minVertex]->end();
               while(startItr != endItr)
                 {
                    if( (vertexAdded[(*startItr)->destinationVertex] == false) && ( (*startItr)->weight < key[(*startItr)->destinationVertex]) )
                       {
                          key[(*startItr)->destinationVertex] = (*startItr)->weight;
                          parent[(*startItr)->destinationVertex] = minVertex;
                       }
                    startItr++;
                 }
            }

            //Calculate cost of minimum spanning tree
            int minimumCostSpanningTree = 0;
            stringstream edges;
            bool skipfirst = true;
            while(currentTree.size() != 0)
              {
                 int vertex = currentTree.front();
                 minimumCostSpanningTree += key[vertex];
                 if(!skipfirst)
                   edges << parent[vertex] << " " << vertex << endl;
                 else
                   skipfirst = false;
                 currentTree.pop_front();
              }
           cout<<minimumCostSpanningTree<<endl;
           cout<<edges.str();
       }

     //Generate a minimum Spanning Tree by Prims algorithm using a Fibonacci Heap
     void getMinimumSpanningTreeUsingPrimsFibonacciScheme(string filepath)
       {
          vector<FibonacciHeapNode*> pointers;
          int key[numberOfVertices];
          FibonacciHeap heap;
          FibonacciHeapNode *temp;
          int parent[numberOfVertices];
          bool vertexAdded[numberOfVertices];

          for(int i=0; i<numberOfVertices; i++)
            {
               temp = new FibonacciHeapNode(new VertexKey(i, 2000));
               heap.insert(temp);
               parent[i] = -1;
               vertexAdded[i]=false;
               pointers.push_back(temp);
               key[i] = 2000;
            }

          list<int> currentTree;

          heap.decreaseKey(pointers[0], 0);
          key[0]=0;
          VertexKey *min;

          while(currentTree.size() < numberOfVertices)
            {
               //Remove Min operation on fibonacci Heap for minimum key
               min = heap.removeMin();

               vertexAdded[min->vertex] = true;
               currentTree.push_back(min->vertex);

               list<EdgeNode*>::iterator startItr = (*adjacencyList)[min->vertex]->begin();
               list<EdgeNode*>::iterator endItr = (*adjacencyList)[min->vertex]->end();
               while(startItr != endItr)
                 {
                    if( (vertexAdded[(*startItr)->destinationVertex] == false) && ( (*startItr)->weight < key[(*startItr)->destinationVertex] ) )
                       {
                          //Decrease key operation for fibonacci Heap for adjacent vertices of min node
                          heap.decreaseKey(pointers[(*startItr)->destinationVertex], (*startItr)->weight);
                          key[(*startItr)->destinationVertex] = (*startItr)->weight;
                          parent[(*startItr)->destinationVertex] = min->vertex;
                       }
                    startItr++;
                 }
            }

            //Calculate minimum spanning tree cost
            int minimumCostSpanningTree = 0;
            stringstream edges;
            bool skipfirst = true;
            while(currentTree.size() != 0)
              {
                 int vertex = currentTree.front();
                 minimumCostSpanningTree += key[vertex];
                 if(!skipfirst)
                   edges << parent[vertex] << " " << vertex << endl;
                 else
                   skipfirst = false;
                 currentTree.pop_front();
              }
           cout<<minimumCostSpanningTree<<endl;
           cout<<edges.str();
       }

     //Generate grpah from reading a file
     bool generateGraphFromFile(string filePath)
       {
          ifstream ifs(filePath.c_str(), std::fstream::in);
          char firstLine[256];
          ifs.getline(firstLine, 256);
          char *pch = strtok(firstLine, " ");
          if(pch != NULL)
            numberOfVertices = atoi(pch);

          reAllocateListWithNewNumberOfVertices(numberOfVertices);
          pch = strtok(NULL, " ");
          int numberOfEdgesToAdd;
          if(pch != NULL)
            numberOfEdgesToAdd = atoi(pch);
          //cout<<"number of edges: "<<numberOfEdgesToAdd;

          int counter=0, startVertex, endVertex, cost;

          //Read edges
          while( counter<numberOfEdgesToAdd && ifs.good() )
            {
               char line[256];
               ifs.getline(line, 256);
               //Get  start vertex, end vertex and cost
               pch = strtok(line, " ");
               if(pch != NULL)
                 startVertex = atoi(pch);
               pch = strtok(NULL, " ");
               if(pch != NULL)
                 endVertex = atoi(pch);
               pch = strtok(NULL, " ");
               if(pch != NULL)
                 cost = atoi(pch);
               //cout<<startVertex<<" "<<endVertex<<" "<<cost<<endl;
               bool present = edgePresentArray[startVertex][endVertex] && edgePresentArray[endVertex][startVertex];
               if(!present)
                 addEdge(startVertex, endVertex, cost);
               else
                 { cout<<"Edge already exists"<<endl;
                 }

               counter=counter+1;
            }

       }

     //Print Graph
     void printGraph()
       {
          cout<<"Number of Vertices: "<<numberOfVertices<<endl;
          cout<<"Number of Edges: "<<numberOfEdges<<endl;
          cout<<endl<<"Edges: "<<endl;
          int i=0;
          int count=0;
          while(i<adjacencyList->size())
            {
               if(nodePresent(i))
                 {
                   list<EdgeNode *>::iterator startItr1 = (*adjacencyList)[i]->begin();
                   list<EdgeNode *>::iterator endItr1 = (*adjacencyList)[i]->end();
                   while( startItr1 != endItr1)
                     {
                       cout<<i<<" "<<(*startItr1)->destinationVertex<<" "<<(*startItr1)->weight<<endl;
                       startItr1++;
                       count++;
                     }
                 }
               i++;
            }
            cout<<"Number of edges in graph is: "<<count;
      }

     //Returns true if node is present
     bool nodePresent(int node)
       {
         if( (*adjacencyList)[node] == NULL )
           return false;
         else
           return true;
       }

     //Compare two edge nodes
     bool compareAdjacencyNode(EdgeNode* first, EdgeNode* second)
       {
         return (first->sourceVertex==second->sourceVertex && first->destinationVertex==second->destinationVertex && first->weight == second->weight);
       }
};



int main(int argc, char * argv[])
{
   struct timeval st;
   struct timeval et;
   struct timezone tp;

   int noOfOptions = argc;

   if(argc < 2)
     {
        cout<<"No options to program. Exiting..";
        exit(1);
     }
   else
     {
       string firstOption = argv[1];
       if(firstOption == "-r")
         {
            if(argc < 4)
              {
                 cout<<"Need more input. Exiting..";
                 exit(0);
              }
            else
              {
                 int numberOfVertices = atoi(argv[2]);
                 int density = atoi(argv[3]);
                 //cout<<numberOfVertices<<" "<<density;

                 //Generate Graph until connected graph Obtained
                 Graph *g = new Graph(numberOfVertices);
                 g->generateRandomGraph(density);
                 while(!g->graphConnectedOrNot())
                   {
                      cout<<"Graph not Connected: "<<endl;
                      g->reAllocateListWithNewNumberOfVertices(numberOfVertices);
                      g->generateRandomGraph(density);
                   }

                 //Minimum Spanning tree using Simple Scheme
                 gettimeofday(&st, &tp);
                 g->getMinimumSpanningTreeUsingPrimsSimpleScheme(" ");
                 gettimeofday(&et, &tp);
                 int s=((et.tv_sec-st.tv_sec)*pow(10,6) + ((et.tv_usec-st.tv_usec)));
                 cout<<"Time taken: "<<s<<" microseconds"<<endl<<endl<<endl;

                 //Minimum Spanning tree using Fibonacci Scheme
                 gettimeofday(&st, &tp);
                 g->getMinimumSpanningTreeUsingPrimsFibonacciScheme(" ");
                 gettimeofday(&et, &tp);
                 int t=((et.tv_sec-st.tv_sec)*pow(10,6) + ((et.tv_usec-st.tv_usec)));
                 cout<<"Time taken: "<<t<<" microseconds"<<endl;

                 //cout<<(s-t)<<endl;

              }
         }
       else if(firstOption == "-s")
         {
            string filePath = argv[2];

            //Generate graph
            Graph *g = new Graph(0);
            g->generateGraphFromFile(filePath);

            //Minimum Spanning tree using Simple Scheme
            gettimeofday(&st, &tp);
            g->getMinimumSpanningTreeUsingPrimsSimpleScheme("filePath");
            gettimeofday(&et, &tp);
            //cout<<((et.tv_sec-st.tv_sec)*pow(10,6) + ((et.tv_usec-st.tv_usec)))<<" microseconds"<<endl;
         }
       else if(firstOption == "-f")
         {
            string filePath = argv[2] ;

            //Generate graph
            Graph *g = new Graph(0);
            bool result = g->generateGraphFromFile(filePath);

            //Minimum Spanning tree using Fibonacci Scheme
            gettimeofday(&st, &tp);
            g->getMinimumSpanningTreeUsingPrimsFibonacciScheme("filepath");
            gettimeofday(&et, &tp);
            //cout<<((et.tv_sec-st.tv_sec)*pow(10,6) + ((et.tv_usec-st.tv_usec)))<<" microseconds"<<endl;
         }
       else
         {
           cout<<"Invalid first argument. Exiting..";
           exit(1);
         }
     }
  return 0;

}
