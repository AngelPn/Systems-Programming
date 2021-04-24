#ifndef SIGNAL_H
#define SIGNAL_H

/* Configure signals */
void signals_config(void);

/* Check if signals are pending and handle */
void signals_check(void);

/* Block signals */
void signals_block(void);

/* Unblock signals */
void signals_unblock(void);

#endif
