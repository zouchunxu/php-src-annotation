	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_CONF_H
#define FPM_CONF_H 1

#include <stdint.h>
#include "php.h"

#define PM2STR(a) (a == PM_STYLE_STATIC ? "static" : (a == PM_STYLE_DYNAMIC ? "dynamic" : "ondemand"))

#define FPM_CONF_MAX_PONG_LENGTH 64

struct key_value_s;

struct key_value_s {
	struct key_value_s *next;
	char *key;
	char *value;
};

/*
 * Please keep the same order as in fpm_conf.c and in php-fpm.conf.in
 */
struct fpm_global_config_s {
	char *pid_file;
	char *error_log;
#ifdef HAVE_SYSLOG_H
	char *syslog_ident;
	int syslog_facility;
#endif
	int log_level;
	int log_limit;
	int log_buffering;
	int emergency_restart_threshold;
	int emergency_restart_interval;
	int process_control_timeout;
	int process_max;
	int process_priority;
	int daemonize;
	int rlimit_files;
	int rlimit_core;
	char *events_mechanism;
#ifdef HAVE_SYSTEMD
	int systemd_watchdog;
	int systemd_interval;
#endif
};

extern struct fpm_global_config_s fpm_global_config;

/*
 * Please keep the same order as in fpm_conf.c and in php-fpm.conf.in
 */
struct fpm_worker_pool_config_s {
    // pool 名称
	char *name;
	char *prefix;
	// fpm 的启动用户
	char *user;
	// fpm的启动用户组
	char *group;
	// 监听地址，配置：listen
	char *listen_address;
	int listen_backlog;
	/* Using chown */
	char *listen_owner;
	char *listen_group;
	char *listen_mode;
	char *listen_allowed_clients;
	int process_priority;
	int process_dumpable;
	// 进程模型: static/dynamic/ondemand
	int pm;
	// 最大worker进程数
	int pm_max_children;
	// 启动初始化的worker数
	int pm_start_servers;
	// 最小空闲worker数
	int pm_min_spare_servers;
	// 最大空闲worker数
	int pm_max_spare_servers;
	// worker空闲时间
	int pm_process_idle_timeout;
	// worker处理的最大请求数
	int pm_max_requests;
	char *pm_status_path;
	char *ping_path;
	char *ping_response;
	char *access_log;
	char *access_format;
	char *slowlog;
	int request_slowlog_timeout;
	int request_slowlog_trace_depth;
	int request_terminate_timeout;
	int rlimit_files;
	int rlimit_core;
	char *chroot;
	char *chdir;
	int catch_workers_output;
	int decorate_workers_output;
	int clear_env;
	char *security_limit_extensions;
	struct key_value_s *env;
	struct key_value_s *php_admin_values;
	struct key_value_s *php_values;
#ifdef HAVE_APPARMOR
	char *apparmor_hat;
#endif
#ifdef HAVE_FPM_ACL
	/* Using Posix ACL */
	char *listen_acl_users;
	char *listen_acl_groups;
#endif
};

struct ini_value_parser_s {
	char *name;
	char *(*parser)(zval *, void **, intptr_t);
	intptr_t offset;
};

enum {
	PM_STYLE_STATIC = 1,
	PM_STYLE_DYNAMIC = 2,
	PM_STYLE_ONDEMAND = 3
};

int fpm_conf_init_main(int test_conf, int force_daemon);
int fpm_worker_pool_config_free(struct fpm_worker_pool_config_s *wpc);
int fpm_conf_write_pid();
int fpm_conf_unlink_pid();

#endif
