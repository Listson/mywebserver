#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define LEN 100
#define MAXBUF 1024
#define DEFAULTROOT "/web-app/mysite"
struct my_int{
	int num;
	int flag;
};
int calculate(char *exp);
int branket(char *exp);
void replace(char *head, char *end, char *src);
int main(int argc, char **argv)
{
	char *buf,*p;
	char express[MAXBUF]="";
	char sendbuf[MAXBUF]="";
	if((buf=getenv("ARG_STRING"))!=NULL){
		if((p = strchr(buf, '='))!=NULL)
			strcpy(express, p+1);
		else
			strcpy(express, buf);
	}
	printf("HTTP/1.1 200 ok\r\n");
	printf("Server: liuxin\r\n");
	printf("Content-Type: text/html; charset=utf-8\r\n\r\n");

	printf("<html><head><title>表达式求值</title><style type=\"text/css\">");
	printf("#Layer1{position:absolute;width:800px;height:500px;left:300px;top:100px;z-index:1;}</STYLE></HEAD>");
	printf("<body background=\"%s/images/cacl.jpg\">", DEFAULTROOT);
	printf("<dir id=\"Layer0\" style=\"text-align:right;\"><a href=\"%s/index.html\"><img src=\"%s/images/back.jpg\" width=\"100\" height=\"70\"></img></a></dir>", DEFAULTROOT, DEFAULTROOT);
	printf("<div id=\"Layer1\" style=\"text-align:center;\">");
	printf("<font color=\"#FF0000\" size=\"3\"><br/><br/><br/>");
	printf("<marquee direction=right>友情提示:同学们不要玩手机,好好上课哦!</marquee></font>");
	printf("<font color=\"#FFFFFFF\" size=\"5\"><br><br><H1>刘老师课堂开课啦!</H1>表达式<br/></font>");
	printf("<form action=\"%s/cgi-bin/cal/cacl\" method=\"get\">", DEFAULTROOT);
	printf("<input type=\"text\" name=\"express\" size=\"33\" value=\"%s\"><br/><br/>",express);
	fflush(NULL);
	if(strlen(express)!=0){
		char *exp = malloc(sizeof(char)*LEN);
		strcpy(exp, express);
		//括号处理
		while(branket(exp));
		int result = calcutate(exp);
		free(exp);
		sprintf(express, "%d", result);
	}
	printf("<input type=\"submit\" value=\"提交\"><input type=\"submit\" value=\"重置\">");
	printf("</form><font color=\"#FFFFFFF\" size=\"5\">结果</font><br>");
	printf("<input type=\"text\" size=\"33\" value=\"%s\"><br><br>", express);
	printf("</div></body></html>");
	fflush(NULL);
}
int branket(char *exp){
	char *p_cur, *p_left, *p_right;
	char tmp[LEN]="";
	for(p_cur = exp; *p_cur&&*p_cur!=')'; p_cur++);
	if(*p_cur=='\0')//没有找到左括号则返回0
		return 0;
	p_right = p_cur;
	while(*p_cur!='(')
		p_cur--;
	p_left = p_cur;
	//找到左右括号，将左右括号中的表达式复制到临时缓存tmp中
	strncpy(tmp, p_left+1, p_right-(p_left+1));
	int res = calcutate(tmp);
	sprintf(tmp, "%d", res);
	//将括号中的表达式题换成计算出来的结果(res)
	replace(p_left, p_right+1, tmp);
	//printf("%s\n", exp);
	return 1;
}
int calcutate(char *exp){
	char *p = exp;
	struct my_int *ppos, *pnext;
	struct my_int *data = malloc(sizeof(struct my_int)*(strlen(exp)+1));	
	pnext = data;
	int up_num;//前一个运算数
	int next_num;//后一个运算数
	//第一次遍历先计算乘除将加减和运算数存入data数组中
	while(*p!='\0'){
		if(*p=='*' || *p=='/'){
			//遇到*/运算符则进行乘除
			up_num = ppos->num;
			next_num = atoi(p+1);
			//printf("up:%d, next:%d\n", up_num, next_num);
			if(*p=='*'){
				ppos->num = up_num*next_num;
			}
			if(*p=='/')
				ppos->num = up_num/next_num;
			p = (p+1) + strspn(p+1, "0123456789")-1;
		}
		else{
			//不是乘除运算则存入data中
			if(*p>='0' && *p<='9'){
				//说明当前遍历到的是运算数
				pnext->num = atoi(p);
				p = p + strspn(p, "0123456789")-1;
			}
			else{
				pnext->num = *p;
				//if((*p=='+')||(*p=='-'&&*(p+1)!='+'&&*(p+1)!='-'&&*(p+1)!='*'&&*(p+1)!='/')){
				pnext->flag = 1;
				//}
			}
			ppos = pnext;
			pnext++;
		}
		p++;
	}
	struct my_int *ptmp = data;
	while(ptmp!=pnext){
		if((ptmp->num=='+'||ptmp->num=='-')&&(ptmp->flag)){
			up_num = data[0].num;
			next_num = (ptmp+1)->num;
			if(ptmp->num=='+')
				data[0].num = up_num+next_num;
			else
				data[0].num = up_num-next_num;
		}
		ptmp++;
	}
	return data[0].num;
}
void replace(char *head, char *end, char *src){
	strcpy(head, end);
	memmove(head+strlen(src), head, strlen(head)+1);
	strncpy(head, src, strlen(src));
}
