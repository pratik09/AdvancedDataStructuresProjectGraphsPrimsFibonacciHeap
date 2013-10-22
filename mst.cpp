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

class FibonacciHeapNode
{
   public:

     int degree;
     FibonacciHeapNode *child;

     //Specific for Prim's Application of Fibonacci Heap
     VertexKey *data;

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

class FibonacciHeap
{
   public:

     int numberOfNodes;
     FibonacciHeapNode *min;

     FibonacciHeap()
       {
         min = NULL;
         numberOfNodes = 0;
       }

     void insert(FibonacciHeapNode *node)
       {
          if(min == NULL)
            {
              min = node;
              min->right=min->left=min;
            }
          else
            {
              node->right = min->right;
              node->left = min;
              min->right->left = node;
              min->right = node;
              if(node->data->key < min->data->key)
                 min = node;
            }
       }

     void print()
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

     //returns pointer to minimum key node in list and sets parent as NULL and childcut as false in all nodes in list
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

     //Combines a list pointed by 
     void combineListWithTopLevelList(FibonacciHeapNode *list)
       {
          if(list == NULL)
            return;

          //Find minimum key node in list
          FibonacciHeapNode *minPointer = minKeyNode(list, true, false);

          //Merge list with top level list
          list->left->right = min->right;
          min->right->left = list->left;
          min->right = list;
          list->left = min;

          //Update min pointer
          if(minPointer->data->key < min->data->key)
             min = minPointer;
       }

     VertexKey* removeMin()
      {
        //no min
        if(min == NULL)
          return NULL;

        //Just one top level root
        if(min->right==min && min->left==min)
          {
             //has no child
             if(min->child == NULL)
              {
                 VertexKey* data = min->data;
                 //destroy Node code
                 min = NULL;
                 return data;
              }
             else  //has childs
              {
                 FibonacciHeapNode *minPointer = minKeyNode(min->child, true, false);
                 VertexKey *data = min->data;
                 min = minPointer;
                 //cout<<"After remove: Before pairwise combine: "<<endl;
                 //print();
                 pairwiseCombine();
                 //cout<<"After Pairwise Combine: "<<endl;
                 //print();
                 return data;
              }
          }
        else  //More than one top level roots
          {
            //remove min from top level list
            min->left->right = min->right;
            min->right->left = min->left;

            FibonacciHeapNode *minTemp = min;
            min = minKeyNode(minTemp->right, false, false);
            //cout<<"After remove: Before combine with top level "<<endl;
            //print();
            combineListWithTopLevelList(minTemp->child);
            //cout<<"After Combine with top level list: Before pairwise combine: "<<endl;
            //print();
            pairwiseCombine();
            //cout<<"After Pairwise Combine: "<<endl;
            return minTemp->data;
          }
      }


      void pairwiseCombine()
        {
           vector<FibonacciHeapNode*> treeOfDegree;
           FibonacciHeapNode *itr = min;

           vector<FibonacciHeapNode*> allTrees;
           allTrees.push_back(itr);
           itr = itr->right;
           while(itr != min)
             {
                allTrees.push_back(itr);
                itr = itr->right;
             }

           vector<FibonacciHeapNode*>::iterator startItr = allTrees.begin();
           vector<FibonacciHeapNode*>::iterator endItr = allTrees.end();
           while(startItr != endItr)
             {
                FibonacciHeapNode * linkedPtr = (*startItr);
                int degree =  linkedPtr->degree;
                while(true)
                  {
                    //cout<<"Degree is1: "<<degree<<" Size is: "<<treeOfDegree.size();
                    //Making Vector big enough to have max degree
                    while(degree >= treeOfDegree.size() )
                         treeOfDegree.push_back(NULL);


                    //cout<<"Degree is2: "<<degree<<" Size is: "<<treeOfDegree.size()<<endl;

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

           //cout<<"After pairwise combine: Before forming root level trees fo different degree: "<<endl;
           //print();

           //create root level list with updated min pointer
           min = NULL;
           int minKey;
           startItr = treeOfDegree.begin();
           endItr = treeOfDegree.end();
           while(startItr != endItr)
             {
                if( (*startItr) != NULL)
                  {
                    //(*startItr)->print();
                    if(min == NULL)
                      {
                         min = (*startItr);
                         min->left = min->right = min;
                         minKey = (*startItr)->data->key;
                      }
                    else
                      {
                         min->right->left = (*startItr);
                         (*startItr)->right = min->right;
                         min->right = (*startItr);
                         (*startItr)->left = min;

                         if( (*startItr)->data->key < minKey )
                            {
                               minKey = (*startItr)->data->key;
                               min = (*startItr);
                            }
                      }
                  }
                startItr++;
             }
          //min->print();
          //min->right->print();
          //print();
          //exit(1);
        }

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

               //if parent points to this child;
               if(nodeToRemove->parent->child = nodeToRemove)
                 {
                   //no siblings. Make parent->child null
                   if(nodeToRemove->right==nodeToRemove && nodeToRemove->left==nodeToRemove)
                     nodeToRemove->parent->child = NULL;
                   else //has siblings
                     nodeToRemove->parent->child = nodeToRemove->right;
                 }

               if(firstNodeDeleted == false)
                 {
                    //Merge childrens to top level list
                    combineListWithTopLevelList(nodeToRemove->child);
                    firstNodeDeleted = true;
                 }
               else
                 {
                    //Merge the node itself to top level list
                    nodeToRemove->right = nodeToRemove;
                    nodeToRemove->left = nodeToRemove;
                    combineListWithTopLevelList(nodeToRemove);
                 }

               nodeToRemove = nodeToRemove->parent;

               if(nodeToRemove != NULL)
                  nodeToRemove->degree = nodeToRemove->degree-1;

              } while(nodeToRemove != NULL && nodeToRemove->childCut == true);

           if(nodeToRemove != NULL)
              nodeToRemove->childCut = true;

           return data;
        }

      void decreaseKey(FibonacciHeapNode *nodeToDecreaseKey, int newKey)
        {
           if(nodeToDecreaseKey == NULL)
             {
               cout<<"Argument is null for descrease key: ..Exiting ";
               exit(0);
             }

           if(nodeToDecreaseKey->data->key < newKey)
             {
               cout<<"Already smaller key present: "<<endl;
               return;
             }

           nodeToDecreaseKey->data->key = newKey;

           //No parent. So no min tree property violation.
           if(nodeToDecreaseKey->parent == NULL)
              return;

           if( newKey >= nodeToDecreaseKey->parent->data->key )
              return;

           FibonacciHeapNode *nodeToRemove = nodeToDecreaseKey;

           //new key is smaller than parent
           do{
               //remove node from list
               nodeToRemove->left->right = nodeToRemove->right;
               nodeToRemove->right->left = nodeToRemove->left;

               //if parent points to this child;
               if(nodeToRemove->parent->child = nodeToRemove)
                 {
                   //no siblings. Make parent->child null
                   if(nodeToRemove->right==nodeToRemove && nodeToRemove->left==nodeToRemove)
                     {
                        nodeToRemove->parent->child = NULL;
                     }
                   else //has siblings
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
               //cout<<nodeToRemove<<endl;
               nodeToRemove->degree -=  1;

              } while(nodeToRemove->parent != NULL && nodeToRemove->childCut == true );

           if(nodeToRemove != NULL)
              nodeToRemove->childCut = true;

        }

      FibonacciHeapNode* linkRoots(FibonacciHeapNode *root1, FibonacciHeapNode *root2)
        {
           if(root2->data->key < root1->data->key)
             {
                FibonacciHeapNode *temp = root1;
                root1 = root2;
                root2 = temp;
             }

           if(root1->child != NULL)
             {
               root2->right = root1->child->right;
               root1->child->right->left = root2;
               root1->child->right = root2;
               root2->left = root1->child;
               root2->parent = root1;
               root2->childCut = false;    //verify
             }
           else
             {
               root1->child = root2;
               root2->parent = root1;
               root2->childCut = false;    //verify
               root2->right = root2;
               root2->left = root2;
             }

           root1->degree += 1;

           return root1;
        }

};

class EdgeNode
{
  public:
    int sourceVertex;
    int destinationVertex;
    int weight;

    EdgeNode(int sourceVertex, int destinationVertex, int weight)
      {
        this->sourceVertex = sourceVertex;
        this->destinationVertex = destinationVertex;
        this->weight = weight;
      }

    bool operator== (EdgeNode &rhs)
      {
        return( ( this->sourceVertex == rhs.sourceVertex ) && ( this->destinationVertex == rhs.destinationVertex ) && ( this->weight == rhs.weight ) );
      }
};

class PrimsNode
{
  public:
    int vertex;
    int parent;
    int cost;

  PrimsNode(int vertex, int parent, int cost)
    {
      this->vertex = vertex;
      this->parent = parent;
      this->cost = cost;
    }
};


class Graph
{

  public:
    int numberOfVertices;
    int numberOfEdges;
    vector<list<EdgeNode *> *> *adjacencyList;
    //int totalBits = numberOfVertices * numberOfVertices;
    bool **edgePresentArray;
    //edgePresent = new (bool*)[numberOfVertices];

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

    void reAllocateListWithNewNumberOfVertices(int numberOfVertices)
     {
       this->numberOfVertices = numberOfVertices;
       delete adjacencyList;
       adjacencyList = new vector<list<EdgeNode *> *>;
       for(int i=0; i<numberOfVertices; i++)
         {
           adjacencyList->push_back(NULL);
         }
       edgePresentArray = new bool* [numberOfVertices];
       for(int i=0;i<numberOfVertices;i++)
         edgePresentArray[i] = new bool[numberOfVertices];
     }
/*
    void addEdge(int row, int col)
     {
       size_t loc = row * numberOfVertices + col;
       edgePresent[loc] = 1;
     }

    bool edgePresent(int row, int col)
     {
       size_t loc = row * numberOfVertices + col;
       ((edgePresent[loc] == 1) ? return true : return false);
     }
*/
    void generateRandomGraph(int density)
     {
        //ofstream ofs("randomGraph", std::fstream::out | std::fstream::app);
        //stringstream line;
        srand(time(NULL));
        double totalEdgesPossible = ( ( numberOfVertices * (numberOfVertices-1 ) ) / (2.00) );
        cout<<"Total Edges possible: "<<totalEdgesPossible<<endl;
        //double densityD = density;
        //double numberOfVerticesD = numberOfVertices;
        double numberOfEdgesToAdd = (density/100.00) * totalEdgesPossible;
        //line << numberOfVertices;
        //line << " ";
        //line << (int)numberOfEdgesToAdd;
        //ofs.write((line.str()).c_str(), 256);
        //ofs << '\n';
        cout<<"Number Of Edges To Add: "<<numberOfEdgesToAdd<<endl;
        cout<<numberOfVertices<<" "<<numberOfEdgesToAdd<<endl;
        double i = 0 ;
        double tryWithNoChanges = 0;
        double lastFound = false;
        while(i< numberOfEdgesToAdd)
          {
             int startVertex = rand() % numberOfVertices;
             int endVertex = rand() % numberOfVertices;
             int cost = (rand() % 1000) + 1;
             //cout<<"Generating Edges: "<<endl;
             //cout<<startVertex<<" "<<endVertex<<" "<<cost<<endl;
             bool present = edgePresentArray[startVertex][endVertex] && edgePresentArray[endVertex][startVertex];
             if((startVertex != endVertex) && !present)//addEdge(startVertex, endVertex, cost) )
                {
                  //stringstream lineS;
                  //lineS << startVertex;
                  //lineS << " ";
                  //lineS << endVertex;
                  //lineS << " ";
                  //lineS << cost;
                  //ofs.write((lineS.str()).c_str(), 256);
                  //ofs << '\n';
                  addEdge(startVertex, endVertex, cost);
                  //cout<<startVertex<<" "<<endVertex<<" "<<cost<<endl;
                  //cout<<i<<endl;
                  i++;
                }

             if(!lastFound && present)
               tryWithNoChanges++;

             if(lastFound && present)
               tryWithNoChanges=1;

             if(tryWithNoChanges == 1000)
               {
                 cout<<"Breaking.."<<endl;
                 cout<<"Percentage Completed: "<< (i/numberOfEdgesToAdd)*100.00<<endl;
                 break;
               }

             lastFound = !present;
             //ofs.close();

          }

        cout<<"Number Of Edges Added: "<<i<<endl;

        if(graphConnectedOrNot())
          cout<<"Random graph is Connected: "<<endl;
        else
          cout<<"Random graph is not Connected: "<<endl;
     }

    bool addEdge(int startVertex, int endVertex, int cost)
       {
    //     if(edgePresent(startVertex, endVertex))
    //        return  false;

    //     if(edgePresentArray[startVertex][endVertex])
    //          return false;

         edgePresentArray[startVertex][endVertex] = edgePresentArray[endVertex][startVertex] = true;

         EdgeNode *forStartVertexList = new EdgeNode(startVertex, endVertex, cost);
         EdgeNode *forEndVertexList = new EdgeNode(endVertex, startVertex, cost);

         if( (*adjacencyList)[startVertex] == NULL )
            {
               list<EdgeNode *> *newList = new list<EdgeNode *>();
               (*adjacencyList)[startVertex] = newList;
               (*adjacencyList)[startVertex]->push_back(forStartVertexList);
            }
         else
            (*adjacencyList)[startVertex]->push_back(forStartVertexList);

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

    bool edgePresent(int start, int end)
       {
         if( (*adjacencyList)[start] != NULL && (*adjacencyList)[end] != NULL)
           {
             bool result1 = false;
             list<EdgeNode *>::iterator startItr1 = (*adjacencyList)[start]->begin();
             list<EdgeNode *>::iterator endItr1 = (*adjacencyList)[start]->end();
             while( startItr1 != endItr1)
               {
                  if( (*startItr1)->destinationVertex == end )
                    result1 = true;
                  startItr1++;
               }

             bool result2 = false;
             list<EdgeNode *>::iterator startItr2 = (*adjacencyList)[end]->begin();
             list<EdgeNode *>::iterator endItr2 = (*adjacencyList)[end]->end();
             while(startItr2 != endItr2)
               {
                  if( (*startItr2)->destinationVertex == start)
                    result2 = true;
                  startItr2++;
               }
             //cout<<"result1: "<<result1<<"result2: "<<result2<<endl;
             return (result1 && result2);
           }
         else
           {
              return false;
           }
       }

    bool graphConnectedOrNot()
       {
          queue<int> verticesQ;
          bool verticesVisited[numberOfVertices];
          for(int i=0; i<numberOfVertices; i++)
            verticesVisited[i] = false;

          srand(time(NULL));
          int sourceVertex = rand() % numberOfVertices;
          cout<<"Source Vertex: "<<sourceVertex<<endl;

          while((*adjacencyList)[sourceVertex] == NULL)
               sourceVertex = rand() % numberOfVertices;

          cout<<"Source Vertex: "<<sourceVertex<<endl;

          verticesQ.push(sourceVertex);
          verticesVisited[sourceVertex] = true;

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

          for(int i=0; i<numberOfVertices; i++)
            if(verticesVisited[i] == false)
              return false;

          return true;
       }

     void getMinimumSpanningTreeUsingPrimsSimpleScheme(string filepath)
       {
          int key[numberOfVertices];
          int parent[numberOfVertices];
          bool vertexAdded[numberOfVertices];
          list<int> currentTree;
          for(int i=0; i<numberOfVertices; i++)
            {
               key[i]=2000;
               parent[i]=-1;
               vertexAdded[i]= false;
            }

          //list<PrimsNode *> list;
          //for(int i=0; i<numberOfVertices; i++)
          //  {
          //    PrimsNode *temp = new PrimsNode(i, -1, 2000);
          //    list.push_back(temp);
          //  }
          //(*list.front())->cost = 0;

          key[0]=0;

          while(currentTree.size() < numberOfVertices)
            {
               int minVertex= -1;
               int minKey= 3000;
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

            int minimumCostSpanningTree = 0;
            while(currentTree.size() != 0)
              {
                 int vertex = currentTree.front();
                 minimumCostSpanningTree += key[vertex];
                 //cout<<parent[vertex]<<" "<<vertex<<endl;
                 currentTree.pop_front();
              }
           cout<<endl<<"Minimum Cost : "<<minimumCostSpanningTree<<endl;
       }

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
               min = heap.removeMin();
               vertexAdded[min->vertex] = true;
               currentTree.push_back(min->vertex);

               list<EdgeNode*>::iterator startItr = (*adjacencyList)[min->vertex]->begin();
               list<EdgeNode*>::iterator endItr = (*adjacencyList)[min->vertex]->end();
               while(startItr != endItr)
                 {
                    if( (vertexAdded[(*startItr)->destinationVertex] == false) && ( (*startItr)->weight < key[(*startItr)->destinationVertex] ) )
                       {
                          heap.decreaseKey(pointers[(*startItr)->destinationVertex], (*startItr)->weight);
                          key[(*startItr)->destinationVertex] = (*startItr)->weight;
                          parent[(*startItr)->destinationVertex] = min->vertex;
                       }
                    startItr++;
                 }
            }

            int minimumCostSpanningTree = 0;
            while(currentTree.size() != 0)
              {
                 int vertex = currentTree.front();
                 minimumCostSpanningTree += key[vertex];
                 //cout<<parent[vertex]<<" "<<vertex<<endl;
                 currentTree.pop_front();
              }
           cout<<endl<<"Minimum Cost : "<<minimumCostSpanningTree<<endl;

       }


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

          while( counter<numberOfEdgesToAdd && ifs.good() )
            {
               char line[256];
               ifs.getline(line, 256);
               //cout<<"Line is: "<<line<<endl;
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
                 { //cout<<"Edge already exists"<<endl;
                 }

               counter=counter+1;
               //cout<<"Line no is: "<<counter;
            }

       }

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

     bool nodePresent(int node)
       {
         if( (*adjacencyList)[node] == NULL )
           return false;
         else
           return true;
       }

     bool compareAdjacencyNode(EdgeNode* first, EdgeNode* second)
       {
         return (first->sourceVertex==second->sourceVertex && first->destinationVertex==second->destinationVertex && first->weight == second->weight);
       }
};



int main(int argc, char * argv[])
{
   struct timeval st;
   struct timeval et;
   struct timeval st1;
   struct timeval et1;

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
                 cout<<numberOfVertices<<" "<<density;
                 Graph *g = new Graph(numberOfVertices);
                 g->generateRandomGraph(density);
                 //clock_t start, end;
                 //start = clock();
                 //cout<<start<<endl;
                 gettimeofday(&st, &tp);
                 g->getMinimumSpanningTreeUsingPrimsSimpleScheme(" ");
                 gettimeofday(&et, &tp);
                 int s,t;
                 s=((et.tv_sec-st.tv_sec)*pow(10,6) + ((et.tv_usec-st.tv_usec)));
                 cout<<s<<endl;
                 //cout<<clock()<<endl;
                 //cout<<clock()-start<<endl;
                 //start = clock();
                 //cout<<"time2:"<<endl;
                 //cout<<start<<endl;
                 gettimeofday(&st, &tp);
                 g->getMinimumSpanningTreeUsingPrimsFibonacciScheme(" ");
                 gettimeofday(&et, &tp);
                 t=((et.tv_sec-st.tv_sec)*pow(10,6) + ((et.tv_usec-st.tv_usec)));
                 cout<<t<<endl;
                 cout<<(s-t);
                 //cout<<clock()<<endl;
                 //cout<<clock()-start<<endl;
                 //g->printGraph();
              }
         }
       else if(firstOption == "-s")
         {
            string filePath = argv[2];
            Graph *g = new Graph(0);
            bool result = g->generateGraphFromFile(filePath);
            gettimeofday(&st, &tp);
            g->getMinimumSpanningTreeUsingPrimsSimpleScheme("filePath");
            gettimeofday(&et, &tp);
            cout<<((et.tv_sec-st.tv_sec)*pow(10,3) + ((et.tv_usec-st.tv_usec)/1000.00))<<endl;
            //g->printGraph();
         }
       else if(firstOption == "-f")
         {
            string filePath = argv[2] ;
            Graph *g = new Graph(0);
            bool result = g->generateGraphFromFile(filePath);
            gettimeofday(&st, &tp);
            g->getMinimumSpanningTreeUsingPrimsFibonacciScheme("filepath");
            gettimeofday(&et, &tp);
            cout<<((et.tv_sec-st.tv_sec)*pow(10,3) + ((et.tv_usec-st.tv_usec)/1000.00))<<endl;
            //g->printGraph();
         }
       else
         {
           cout<<"Invalid first argument. Exiting..";
           exit(1);
         }
     }
  return 0;

/*
   Graph g(5);
   bool res;
   res = g.addEdge(2,3,69);
   if (res)
     cout<<"true"<<endl;
   res = g.addEdge(3,4,70);
   if (res)
     cout<<"true"<<endl;
   res = g.addEdge(1,2,89);
   if (res)
     cout<<"true"<<endl;
   //res = g.addEdge(0,4,89);
   //if (res)
     cout<<"true"<<endl;

   g.printGraph();
   cout<<endl<<"Graph Connected? : ";
   if (g.graphConnectedOrNot() )
      cout<<"Connected "<<endl;
   else
      cout<<"Not Connected"<<endl;

*/
   //return 0;

/*
   FibonacciHeap heap;
   VertexKey *min;
   int i=0;
   srand(time(NULL));
   while(i<1000)
   {
      int ran = (rand() % 2000)-1000;
      heap.insert(new FibonacciHeapNode(new VertexKey(2,ran)));
      i++;
   }
   i=0;
   min = heap.removeMin();
   while(min != NULL)
     {  i++;
        min = heap.removeMin();
        if(min != NULL)
          cout<<"MIn is: "<<min->key<<endl;
     }
  cout<<endl<<"i: "<<i;
*/
/*
  FibonacciHeap heap;
  VertexKey *min;
  FibonacciHeapNode node1(new VertexKey(2,4));
  heap.insert(&node1);
  FibonacciHeapNode node2(new VertexKey(2,-3));
  heap.insert(&node2);
  FibonacciHeapNode node3(new VertexKey(2,-1));
  heap.insert(&node3);
  FibonacciHeapNode node4(new VertexKey(2,2));
  heap.insert(&node4);
  FibonacciHeapNode node5(new VertexKey(2,0));
  heap.insert(&node5);
  FibonacciHeapNode node6(new VertexKey(2,7));
  heap.insert(&node6);
  FibonacciHeapNode node7(new VertexKey(2,-2));
  heap.insert(&node7);
  FibonacciHeapNode node8(new VertexKey(2,9));
  heap.insert(&node8);
  FibonacciHeapNode node9(new VertexKey(2,1));
  heap.insert(&node9);
  heap.print();
  min = heap.removeMin();
  cout<<"MIN: "<<min->key<<endl;
  heap.print();

  heap.decreaseKey(&node8, -1);
  heap.decreaseKey(&node9, -2);
  heap.decreaseKey(&node1, -5);
  heap.print();

   min = heap.removeMin();
   cout<<"MIn is: "<<min->key<<endl;
   while(min != NULL)
     {
        min = heap.removeMin();
        if(min != NULL)
          cout<<"MIn is: "<<min->key<<endl;
     }
*/
}
