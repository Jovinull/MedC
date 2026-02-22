CC      ?= cc
CFLAGS  ?= -std=c11 -O2 -g -Wall -Wextra -Wpedantic
CPPFLAGS?= -Isrc
LDFLAGS ?=

SRC := src/main.c \
       src/app/app.c src/app/router.c src/app/session.c \
       src/core/ansi.c src/core/datetime.c src/core/fs.c src/core/input.c src/core/str.c \
       src/modules/admin/admin_controller.c src/modules/admin/admin_views.c \
       src/modules/cadastros/patients_controller.c src/modules/cadastros/patients_views.c \
       src/modules/consultorio/consult_controller.c src/modules/consultorio/consult_views.c \
       src/modules/enfermagem/triage_controller.c src/modules/enfermagem/triage_views.c \
       src/modules/farmacia/pharmacy_controller.c src/modules/farmacia/pharmacy_views.c \
       src/modules/indicadores/indicators_controller.c src/modules/indicadores/indicators_views.c \
       src/modules/recepcao/recepcao_controller.c src/modules/recepcao/recepcao_views.c \
       src/security/auth.c src/security/password.c src/security/rbac.c src/security/sha256.c \
       src/storage/db.c src/storage/migrations.c src/storage/repo_audit.c src/storage/repo_medical.c \
       src/storage/repo_meds.c src/storage/repo_patients.c src/storage/repo_roles.c src/storage/repo_rx.c \
       src/storage/repo_stock.c src/storage/repo_triage.c src/storage/repo_users.c src/storage/repo_visits.c \
       src/ui/layout.c src/ui/ui.c src/ui/widgets.c

OBJ := $(SRC:.c=.o)
BIN := hosp_mvp

.PHONY: build run clean

build: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

run: build
	./$(BIN)

clean:
ifeq ($(OS),Windows_NT)
	@cmd /c "del /S /Q src\*.o 2>NUL" || ver >NUL
	@cmd /c "del /Q hosp_mvp.exe hosp_mvp 2>NUL" || ver >NUL
else
	rm -f $(OBJ) $(BIN)
endif
