#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "demux.h"

layer layer_list[] = {
  {physical, "physical"},
  {link, "link"},
  {network, "network"},
  {transport, "transport"},
  {session, "session"},
  {0, NULL}
};

family family_list[] = {
  {FAMILY_ROOT, enet, physical, "ethernet"},
  {FAMILY_ROOT, LSAP, link, "lsap"},
  {FAMILY_ROOT, LSAPUnknown, link, "linkunknown"},
  {FAMILY_ROOT, DDP, network, "ddp"},
  {FAMILY_ROOT, ARP, network, "arp"},
  {FAMILY_ROOT, IP, network, "ip"},
  {FAMILY_ROOT, Xerox, network, "xerox"},
  {FAMILY_ROOT, DSAP, network, "dsap"},
  {FAMILY_ROOT, DECLAT, network, "lat"},
  {FAMILY_ROOT, DECLANBridge, network, "lanbridge"},
  {FAMILY_ROOT, DECMOPRemoteConsole, network, "decremotecons"},
  {FAMILY_ROOT, DECLAVC, network, "declavc"},
  {FAMILY_ROOT, DECNETphIVRoute, network, "decphIVroute"},
  {FAMILY_ROOT, unknownNetwork, network, "networkunknown"},
  {0, 0, NULL}
};

operator op_list[] = {
  {eqaulity, "="},
  {inequality, "!="},
  {0, NULL}
};

cond *AddCondition(cond* list, char *left_str, char *op_str, char *right_str)
{
  cond cd;
  int i;

  for (cd.left = i = 0; layer_list[i].value != 0; i++)
    if (!strcmp(layer_list[i].str, left_str))
      cd.left = layer_list[i].value;

  for (cd.operator = i = 0; op_list[i].value != 0; i++)
    if (!strcmp(op_list[i].str, op_str))
      cd.operator = op_list[i].value;
  
  for (cd.right = i = 0; family_list[i].value != 0; i++)
    if (!strcmp(family_list[i].str, right_str))
      cd.right = family_list[i].value;

  if(!cd.left)
    fprintf(stderr, "Unknown type %s.\n", left_str);
  else if(!cd.operator)
    fprintf(stderr, "Unknown operator %s.\n", op_str);
  else if(!cd.right)
    fprintf(stderr, "Unknown value %s.\n", right_str);
  else
    {
      cond *tmp = malloc(sizeof(cond));
      *tmp = cd;
      tmp->next = list;
      return tmp;
    }
  return list;
}

/* ----------------------------------------------------- */

int check_list(list *l, cond *c)
{
  int i, accept;
  feature *f = (feature*)(l->head);
  cond *tmp = c;
  
  accept = 1;
  for (i = 0; family_list[i].value != 0; i++)
    if (f->type == family_list[i].value)
      {
	accept = 0;
	while (tmp && !accept)
	  {
	    if ((tmp->right == f->type) &&
		(tmp->left == family_list[i].root))
	      accept = 1;
	    tmp = tmp->next;
	  }
      }
  if (l->tail) accept = accept * check_list(l->tail, c);
  return accept;
}

/* ----------------------------------------------------- */
