/*  Grupo 039
 * Rafael Ribeiro   - 58193
 * Pedro Duque      - 52753
 * Francisco Santos - 59871
*/ 
#ifndef HOSPSIGNAL_H_GUARD
#define HOSPSIGNAL_H_GUARD

/* Função que define o comportamento do processo para executar a função signal_ctrl_c
* quando é enviado o sinal SIGINT
*/
void set_sigint();

/* Função que define o comportamento do processo para ignorar o sinal SIGINT quando este 
* é enviado. Esta função deve ser usada para que os filhos do processo pai ignorem este
* sinal, sendo que apenas o processo pai deve tratar do sinal SIGINT
*/
void signal_ignore();

/* Função que é executada quando é enviado o sinal SIGINT, que por si executa a função
* end_execution, e termina executação de hOSpital. Apenas o processo pai tem a capacidade
* de terminar a executação
*/
void ctrl_c();

#endif