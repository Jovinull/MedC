#include "app/app.h"
#include "storage/migrations.h"

int main(void) {
  migrations_ensure_data_dir();
  migrations_seed_if_needed();

  App app;
  app_init(&app);
  app_run(&app);
  app_shutdown(&app);
  return 0;
}