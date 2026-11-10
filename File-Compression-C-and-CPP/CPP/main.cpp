#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <unordered_map>
#include <string>
#include <cstdint>
#include <filesystem>
using namespace std;
struct Node{int ch;uint64_t f;Node*l,*r;Node(int c,uint64_t n):ch(c),f(n),l(nullptr),r(nullptr){}Node(uint64_t n,Node*a,Node*b):ch(-1),f(n),l(a),r(b){}bool leaf(){return !l&&!r;}};
struct Cmp{bool operator()(Node*a,Node*b){return a->f>b->f;}};
void clear(Node*n){if(n){clear(n->l);clear(n->r);delete n;}}
Node* build(vector<uint64_t>&f){priority_queue<Node*,vector<Node*>,Cmp>q;for(int i=0;i<256;i++)if(f[i])q.push(new Node(i,f[i]));if(q.empty())return nullptr;while(q.size()>1){auto*a=q.top();q.pop();auto*b=q.top();q.pop();q.push(new Node(a->f+b->f,a,b));}return q.top();}
void codes(Node*n,string s,unordered_map<int,string>&m){if(!n)return;if(n->leaf()){m[n->ch]=s.empty()?"0":s;return;}codes(n->l,s+"0",m);codes(n->r,s+"1",m);}
bool compress(string src,string dst){ifstream in(src,ios::binary);if(!in){cerr<<"Cannot open input.\\n";return false;}vector<unsigned char>d((istreambuf_iterator<char>(in)),{});vector<uint64_t>f(256);for(auto c:d)f[c]++;Node*r=build(f);unordered_map<int,string>m;codes(r,"",m);ofstream o(dst,ios::binary);if(!o){clear(r);return false;}uint64_t sz=d.size();o.write((char*)&sz,sizeof sz);for(auto x:f)o.write((char*)&x,sizeof x);unsigned char b=0;int n=0;for(auto c:d)for(char bit:m[c]){b=(b<<1)|(bit=='1');if(++n==8){o.put(b);b=0;n=0;}}if(n){b<<=(8-n);o.put(b);}o.close();clear(r);cout<<"Compressed. Original "<<sz<<" bytes; output "<<filesystem::file_size(dst)<<" bytes.\\n";return true;}
bool decompress(string src,string dst){ifstream in(src,ios::binary);uint64_t sz;if(!in.read((char*)&sz,sizeof sz)){cerr<<"Invalid input.\\n";return false;}vector<uint64_t>f(256);for(auto&x:f)if(!in.read((char*)&x,sizeof x)){cerr<<"Invalid header.\\n";return false;}Node*r=build(f);ofstream o(dst,ios::binary);if(!o){clear(r);return false;}if(sz==0){clear(r);cout<<"Restored empty file.\\n";return true;}if(!r)return false;Node*cur=r;uint64_t w=0;char raw;while(in.get(raw)&&w<sz){unsigned char b=raw;for(int i=7;i>=0&&w<sz;i--){cur=((b>>i)&1)?cur->r:cur->l;if(!cur){clear(r);return false;}if(cur->leaf()){o.put((char)cur->ch);w++;cur=r;}}}o.close();clear(r);cout<<(w==sz?"Decompression completed.\\n":"Incomplete data.\\n");return w==sz;}
int main(){int ch;string in,out;cout<<"Huffman File Tool (C++)\\n1.Compress  2.Decompress\\nChoice: ";cin>>ch;cout<<"Input path: ";cin>>in;cout<<"Output path: ";cin>>out;if(ch==1)return compress(in,out)?0:1;if(ch==2)return decompress(in,out)?0:1;cout<<"Invalid choice.\\n";return 1;}
