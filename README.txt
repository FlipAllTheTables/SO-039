Este projeto foi desenvolvido por:
    Rafael Ribeiro, número 58193
    Pedro Duque, número 52753
    Francisco Santos, número 59871

Como executar:
    1 - Entrar num terminal, e seguir para a pasta HOSPITAL
    2 - Inserir o comando $make
    3 - Criar um ficheiro de configuração que será passado como argumento
        O ficheiro tem o seguinte formato:
            max_ads                 //nº máximo de admissões
            buffers_size            //tamanho dos buffers
            n_patients              //nº de pacientes
            n_receptionists         //nº de rececionistas
            n_doctors               //nº de médicos
            log_filename            //nome do ficheiro de log
            statistics_filename     //nome do ficheiro de estatísticas
            alarm_time              //temporização para o alarme

    4 - Executar o executável hOSpital com o comando $./bin/hOSpital config_file
    5 - Inserir comandos no terminal de acordo com o que o utilizador deseja
    6 - Verificar que cada operação válida é escrita no ficheiro log
    7 - Inserir "end" ou Ctrl+C para terminar executação de hOSpital
    8 - Verificar resultados finais da executação de hOSpital no ficheiro stats
    9 - Inserir o comando $make clean para apagar os ficheiros gerados com o comando $make

Limitações de implementação:
    Esta implementação não permite a criação de admissões para além do limite decidido pela váriavel "max_ads".
    Sendo assim, não é possível que uma admissão acabe com o estado 'N', pois este estado apenas acontece se a admissão
    ultrapassar o limite definido.