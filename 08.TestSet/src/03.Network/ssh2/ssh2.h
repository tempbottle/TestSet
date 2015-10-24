/*
 * Sample showing how to do SSH2 connect.
 *
 * The sample code has default values for host name, user name, password
 * and path to copy, but you can specify them on the command line like:
 *
 * "ssh2 host user password [-p|-i|-k]"
 */

#ifndef TESTSET_SSH2
#define TESTSET_SSH2

typedef int (*ssh_recv_data_callback)(void *param, const char *data, int data_len);

enum
{
    SSH_TYPE_SSH,
    SSH_TYPE_SFTP,
};

typedef struct _tag_my_ssh_param
{
    int type;       // 0-ssh,1-sftp
    int exit;       // 0-normal,1-exit
    void *param;    // client socket
    void *param1;   // local listent port, local client port
    void *param2;   // remote port
    void *param3;
    void *session;
    void *channel;

    const char *server_addr;
    unsigned short server_port;
    const char *username;
    const char *password;

    ssh_recv_data_callback recv_cb; // ssh�ص�

}my_ssh_param, *p_my_ssh_param;

/**
 *\fn        void ssh_thread_func(void *param)
 *\brief     ssh���߳�
 *\param[in] void * param p_my_ssh_param
 *\return    void 0�ɹ�,����ʧ��
 */
void ssh_thread_func(void *param);

/**
 *\fn        int ssh_send_data(my_ssh_param *param, const char *data, int len)
 *\brief     ssh��������
 *\param[in] my_ssh_param * param ssh��������
 *\param[in] const char * data Ҫ���͵�����
 *\param[in] int len Ҫ���͵����ݳ�
 *\return    int 0�ɹ�,����ʧ��
 */
int ssh_send_data(my_ssh_param *param, const char *data, int len);

/**
 *\fn           int ssh_set_size(my_ssh_param *param, int width, int height)
 *\brief        ������Ļ���
 *\param[in]    my_ssh_param * param ssh��������
 *\param[in]    int width ��Ļ��
 *\param[in]    int height ��Ļ��
 *\return       int 0�ɹ�,����ʧ��
 */
int ssh_set_size(my_ssh_param *param, int width, int height);

#endif
