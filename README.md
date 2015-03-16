pg_check_app_audit
==================

**pg_check_app_audit** é uma pequena biblioteca que visa fazer com que o **Postgres** possa limitar o acesso de usuários por meio da especificação de aplicações que poderão ser utilizadas para as conexões destes com a base de dados.
A ideia é limitar conexões de um usuário que não esteja dentro de uma lista em que o nome da aplicação faça parte.

I. INSTALAÇÃO
-------------

1. Copiar a pasta **pg_check_app_audit** para `<pg_source_dir>/contrib`
1. Entrar na pasta
1. Executar: `make && make install`

II. CONFIGURAÇÃO
----------------

- Abrir o arquivo `postgresql.conf`.

- Localizar o parâmetro `shared_preload_libraries` e adicionar a *lib* **pg_check_app_audit**.

  Ex.:

  ```sh
  shared_preload_libraries = '$libdir/pg_check_app_audit'
  ```

- Criar uma *custom_variable* chamada `pg_check_app_audit.users_apps`.

  Ex.:

  ```sh
  # Configuração da lib **pg_check_app_audit**
  pg_check_app_audit.users_apps = 'postgres:psql,clusterdb'
  ```

- O preenchimento da variável acima deve seguir o seguinte padrão:

  `user_name1:app1[,app2, ...][;user_name2:app1[,app2, ...]][...]`

  > Onde:
  >
  > + `user_name`: Nome do usuário.
  >
  > + `app`: Nome do aplicativo em que a conexão será permitida.
  >
  > Os nomes dos *apps* são separados por "`,`" (vírgula).
  >
  > Cada configuração de usuário é separada por "`;`" (ponto-e-vírgula).

- Para que as configurações tenham efeito a base de dados deverá ser reiniciada.

III. BUGS
----------

Favor reportar eventuais bugs para:

| E-mail                 | Celular                               | Skype        | Twitter                                          |
| ---------------------- | ------------------------------------- | ------------ | ------------------------------------------------ |
| marconeperes@gmail.com | [+55 61 8146-0028](tel:+556181460028) | marconeperes | [@marconeperes](http://twitter.com/marconeperes) |