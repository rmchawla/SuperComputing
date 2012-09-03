//#include <iostream>
#include <fstream>
#include <iostream>

using namespace std;

int main()
{

int u,v,w;
int edges,vertices;
cin>>vertices>>edges;
cout<<edges<<"  "<<vertices<<endl;
      for(int i=0 ; i<edges ; i++) {
       cin >>u>>v>>w;
       cout<<u<<" "<<v<<" "<<w<<endl;
   }

return 0;
}
