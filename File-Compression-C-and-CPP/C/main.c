#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#define A 256
#define MAXN 511
typedef struct { int ch; unsigned long long f; int l,r; } Node;
Node t[MAXN]; int nn=0, h[A], hs=0;
int make_node(int ch,unsigned long long f,int l,int r){int x=nn++;t[x]=(Node){ch,f,l,r};return x;}
void push(int x){int i=hs++;while(i){int p=(i-1)/2;if(t[h[p]].f<=t[x].f)break;h[i]=h[p];i=p;}h[i]=x;}
int popmin(){int z=h[0],v=h[--hs],i=0;while(2*i+1<hs){int c=2*i+1;if(c+1<hs&&t[h[c+1]].f<t[h[c]].f)c++;if(t[v].f<=t[h[c]].f)break;h[i]=h[c];i=c;}if(hs)h[i]=v;return z;}
int leaf(int x){return t[x].l==-1&&t[x].r==-1;}
int build(unsigned long long f[A]){nn=hs=0;for(int i=0;i<A;i++)if(f[i])push(make_node(i,f[i],-1,-1));if(!hs)return -1;while(hs>1){int a=popmin(),b=popmin();push(make_node(-1,t[a].f+t[b].f,a,b));}return popmin();}
char code[A][A];
void codes(int x,char s[],int d){if(leaf(x)){if(!d)s[d++]='0';s[d]=0;strcpy(code[t[x].ch],s);return;}s[d]='0';codes(t[x].l,s,d+1);s[d]='1';codes(t[x].r,s,d+1);}
void compress_file(char *src,char *dst){FILE *in=fopen(src,"rb");if(!in){puts("Cannot open input.");return;}unsigned long long f[A]={0},size=0;int c;while((c=fgetc(in))!=EOF){f[(unsigned char)c]++;size++;}rewind(in);int root=build(f);memset(code,0,sizeof code);if(root>=0){char s[A];codes(root,s,0);}FILE*out=fopen(dst,"wb");if(!out){puts("Cannot create output.");fclose(in);return;}fwrite(&size,sizeof size,1,out);fwrite(f,sizeof f[0],A,out);unsigned char b=0;int n=0;while((c=fgetc(in))!=EOF){char*s=code[(unsigned char)c];for(int i=0;s[i];i++){b=(b<<1)|(s[i]=='1');if(++n==8){fputc(b,out);b=0;n=0;}}}if(n){b<<=(8-n);fputc(b,out);}fclose(in);fclose(out);puts("Compression completed.");}
void decompress_file(char *src,char *dst){FILE*in=fopen(src,"rb");if(!in){puts("Cannot open compressed file.");return;}unsigned long long size,f[A];if(fread(&size,sizeof size,1,in)!=1||fread(f,sizeof f[0],A,in)!=A){puts("Invalid file.");fclose(in);return;}int root=build(f);FILE*out=fopen(dst,"wb");if(!out){puts("Cannot create output.");fclose(in);return;}if(!size){puts("Restored empty file.");fclose(in);fclose(out);return;}if(root<0){puts("Invalid tree.");fclose(in);fclose(out);return;}int cur=root,w=0,b;while((b=fgetc(in))!=EOF&&w<size){for(int i=7;i>=0&&w<size;i--){cur=((b>>i)&1)?t[cur].r:t[cur].l;if(leaf(cur)){fputc(t[cur].ch,out);w++;cur=root;}}}fclose(in);fclose(out);puts(w==size?"Decompression completed.":"Incomplete data.");}
int main(){int ch;char in[512],out[512];printf("Huffman File Tool (C)\\n1.Compress  2.Decompress\\nChoice: ");if(scanf("%d",&ch)!=1)return 1;printf("Input path: ");scanf("%511s",in);printf("Output path: ");scanf("%511s",out);if(ch==1)compress_file(in,out);else if(ch==2)decompress_file(in,out);else puts("Invalid choice.");return 0;}
