#ifndef STORAGE_MIGRATIONS_H
#define STORAGE_MIGRATIONS_H

void migrations_ensure_data_dir(void);
void migrations_seed_if_needed(void);

#endif