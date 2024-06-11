#include "TMUX131.h"

void TMUX131::EnableSender()//fromPICOtoTWE
{
  digitalWrite(SEL0, HIGH);
  digitalWrite(SEL1, LOW);
}
void TMUX131::EnableReceiver()//fromTWEtoPICO
{
  digitalWrite(SEL0, LOW);
  digitalWrite(SEL1, HIGH);
}
void TMUX131::EnableUSB()
{
  digitalWrite(SEL0, LOW);
  digitalWrite(SEL1, LOW);
}
void TMUX131::Disable()
{
  digitalWrite(SEL0, HIGH);
  digitalWrite(SEL1, HIGH);
}
