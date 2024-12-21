#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "ini.h"
#include "dyad.h"

char *host="irc.undernet.org";
int   port=6667;
char *nick="siesto";
char *chan="#pantasya";


bool isregistered = false;

int ticks = 0;

typedef struct IrcMsg IrcMsg;

struct IrcMsg {
  char *cmd,*usr,*par,*txt;
};


static char *eat(char *s, int (*p)(int), int r) {
	while(*s != '\0' && p((unsigned char)*s) == r)
		s++;
	return s;
}

static char *skip(char *s, char c) {
	while(*s != c && *s != '\0')
		s++;
	if(*s != '\0')
		*s++ = '\0';
	return s;
}

static void trim(char *s) {
	char *e;

	for (e = s + strlen(s); e > s && isspace((unsigned char)*(e - 1)); e--)
		;
	*e = '\0';
}

static IrcMsg *ircparse(char *line) {

  char *cmd,*usr,*par,*txt;

  IrcMsg *im=malloc(sizeof(*im));

  cmd = line;
	usr = host;
	if(!cmd || !*cmd)
		return NULL;
	if(cmd[0] == ':') {
		usr = cmd + 1;
		cmd = skip(usr, ' ');
		if(cmd[0] == '\0')
			return NULL;
		skip(usr, '!');
	}
	skip(cmd, '\r');
	par = skip(cmd, ' ');
	txt = skip(par, ':');
	trim(par);

  im->cmd=cmd;
  im->usr=usr;
  im->par=par;
  im->txt=txt;

  return im;
}



static void onConnect(dyad_Event *e) {
  /* Generate a random nick name */
  sprintf(nick, "testbot%04x", (int)(dyad_getTime()) % 0xFFFF);
  /* Introduce ourselves to the server */
  dyad_writef(e->stream, "NICK %s\r\n", nick);
  dyad_writef(e->stream, "USER %s %s bla :%s\r\n", nick, nick, nick);
}

static void onError(dyad_Event *e) {
  printf("error: %s\n", e->msg);
}

static void onLine(dyad_Event *e) {

  printf("%s\n",e->data);

  IrcMsg *im=ircparse(e->data);

/*
  printf("usr: %s\n",im->usr);
  printf("cmd: %s\n",im->cmd);
  printf("par: %s\n",im->par);
  printf("txt: %s\n",im->txt);
*/

  if(!strcmp(im->cmd,"PING")) {
    dyad_writef(e->stream, "PONG :%s\r\n", im->txt);
  } else if(!isregistered && !strcmp(im->cmd,"001")) {
    dyad_writef(e->stream, "JOIN %s\r\n", chan);
    isregistered=true;
  }

  free(im);
  im=NULL;

}

static void onTick(dyad_Event *e) {
// printf("Ticks: %i\n", ++ticks);
}


int main(void) {



  ini_t *config = ini_load("config.ini");

  host = (char*)ini_get(config, NULL,"host");
  nick = (char*)ini_get(config, NULL,"nick");
  chan = (char*)ini_get(config, NULL,"chan");
  ini_sget(config, NULL,"port","%d",&port);

  ini_free(config);



  dyad_Stream *s;
  dyad_init();

  s = dyad_newStream();
  dyad_addListener(s, DYAD_EVENT_CONNECT, onConnect, NULL);
  dyad_addListener(s, DYAD_EVENT_ERROR,   onError,   NULL);
  dyad_addListener(s, DYAD_EVENT_LINE,    onLine,    NULL);
  dyad_addListener(s, DYAD_EVENT_TICK,    onTick,    NULL);
  dyad_setTickInterval(1.0);
  dyad_connect(s, host, port);


  while (dyad_getStreamCount() > 0) {
    dyad_update();
  }


  dyad_shutdown();
  return 0;
}
