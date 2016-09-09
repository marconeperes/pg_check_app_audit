/*
 * Hook para autenticação junto ao PostgreSQL
 *
 * Esta implementação possibilitará que um determinado usuário acesse a base de dados somente via uma aplicação específica
 *
 * Por: Marcone Peres - DBA
 * Em : 2015-03-14
 *
 * Bugs:
 * 		marconeperes@gmail.com
 * 		+55 61 8146-0028
 * 		Skype: marconeperes
 * 		Twitter: @marconeperes
 */

/************************************************************************************************/
// Seção de inclusão de bibliotecas
#include "postgres.h"
#include "libpq/auth.h"
#include "utils/guc.h"
/************************************************************************************************/

// Esta variável será usada para fazer um cache do possível méthodo já usdado para autenticação.
static ClientAuthentication_hook_type original_client_auth_hook = NULL;

/*
 * get_appname retorna o nome da aplicação a qual está tentando conectar-se à base de dados
 */
static char* get_appname(List *guc_options) {
	ListCell *options = guc_options->head;// Celula da lista com a configuração! ASSUMIMOS QUE guc_options JÁ ESTARÁ INICIALIZADA!

	while (options) {			// Percorre a lista de opções...
		if (strcmp((char *) (options->data.ptr_value), "application_name")) {
			return ((char *) (options->data.ptr_value));
		}
		options = options->next;
	}

	return NULL;		// Se não achar a configuração retorna NULL
}

/*
 * strPos retorna a posição de uma substring em uma string
 */
static int strPos(char *str, char *search) {
	char *subst = strstr(str, search);
	if (subst)
		return (subst - str);

	return -1;
}

/*
 * check_connection_app Verifica se o usuário pode logar com a aplicação especificada
 */
static void check_connection_app(Port *port, int status) {
	// Variável de configuração
	char *chk_var;				// Variável de configuração deste Hook

	// Se já existia outro plugin de conexão, chama-o!
	if (original_client_auth_hook)
		original_client_auth_hook(port, status);

	// Captura o valor da variável setada no postgresql.conf
	chk_var = GetConfigOptionByName("pg_check_app_audit.users_apps", NULL);

	// Captura a posição do nome do usuário na configuração
	int user_position = strPos(chk_var, port->user_name);
	int end_user_config;
	int app_position;

	if (user_position >= 0) {// Existe configuração para o usuário que está logando
		// captura a posição do fim da configuração do usuário. O fim da configuração é o ";" ou o fim da string, neste caso strPos retornando -1
		end_user_config = strPos(chk_var + user_position, ";");
		// captura a posição do nome da plicação na configuração.
		app_position = strPos(chk_var + user_position,
				get_appname(port->guc_options));

		// Verifica se application_name está dentro da configuração
		if (end_user_config >= 0 ?
				(app_position < end_user_config && app_position >= 0) :
				app_position < 0) {
			ereport(FATAL,
					(errmsg("Sem entrada em postgresql.conf para usuário \"%s\" com a aplicação \"%s\"", port->user_name, get_appname(port->guc_options))));
		}
	}
}

/*
 * Macro que informa ao postgres que o método _PG_init está disponível para execuação.
 */
PG_MODULE_MAGIC
;
void _PG_init(void);

/*
 * Função de "Load Callback" do Hook de autenticação
 */
void _PG_init(void) {

	// Captura o Hook de autenticação atual
	original_client_auth_hook = ClientAuthentication_hook;
	// Instala nosso Hook de autenticação!!!
	ClientAuthentication_hook = check_connection_app;
}

