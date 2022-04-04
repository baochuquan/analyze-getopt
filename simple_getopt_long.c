#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "getopt.h"

char *optarg;
int optind = 1, opterr = 1, optopt, __optpos, optreset = 0;

#define optpos __optpos

static void __getopt_msg(const char *a, const char *b, const char *c, size_t l)
{
    FILE *f = stderr;
#if !defined(WIN32) && !defined(_WIN32)
    flockfile(f);
#endif
    fputs(a, f);
    fwrite(b, strlen(b), 1, f);
    fwrite(c, 1, l, f);
    fputc('\n', f);
#if !defined(WIN32) && !defined(_WIN32)
    funlockfile(f);
#endif
}

int getopt(int argc, char * const argv[], const char *optstring)
{
    int i, c, d;
    int k, l;
    char *optchar;

    // 如果 optind == 0 或 optreset == 1，则进行复位。
    if (!optind || optreset) {
        optreset = 0;
        __optpos = 0;
        optind = 1;
    }

    // 校验参数数量和内容是否正确
    if (optind >= argc || !argv[optind])
        return -1;

    // 如果 argv[optind] 的第一个字符不为 '-'
    if (argv[optind][0] != '-') {
        // 判断 optstring 的一个字符是否为 '-'
        if (optstring[0] == '-') {
            optarg = argv[optind++];
            return 1;
        }
        return -1;
    }

    // 如果 argv[optind] 的第二个字符为空，则返回 -1
    if (!argv[optind][1])
        return -1;

    // 如果 argv[optind] 的第二个字符为 '-' 且 argv[optind] 的第三个字符为空，则更新选项索引，并返回 -1
    // 猜测处理选项为 '--' 的情况
    if (argv[optind][1] == '-' && !argv[optind][2])
        return optind++, -1;

    if (!optpos) optpos++;
    c = argv[optind][optpos], k = 1;
    optchar = argv[optind]+optpos;
    optopt = c;
    optpos += k;

    if (!argv[optind][optpos]) {
        optind++;
        optpos = 0;
    }

    if (optstring[0] == '-' || optstring[0] == '+')
        optstring++;

    i = 0;
    d = 0;
    do {
        d = optstring[i], l = 1;
        if (l>0) i+=l; else i++;
    } while (l && d != c);

    if (d != c) {
        if (optstring[0] != ':' && opterr)
            __getopt_msg(argv[0], ": unrecognized option: ", optchar, k);
        return '?';
    }
    if (optstring[i] == ':') {
        if (optstring[i+1] == ':') optarg = 0;
        else if (optind >= argc) {
            if (optstring[0] == ':') return ':';
            if (opterr) __getopt_msg(argv[0], ": option requires an argument: ", optchar, k);
            return '?';
        }
        if (optstring[i+1] != ':' || optpos) {
            optarg = argv[optind++] + optpos;
            optpos = 0;
        }
    }
    return c;
}

// 给定一个字符串数组，将src位置的字符串插入到dest的位置，中间的字符串均后移一位
// 要求：src > dest
static void permute(char *const *argv, int dest, int src)
{
    char **av = (char **)argv;
    char *tmp = av[src];
    int i;
    // src > dest
    for (i = src; i > dest; i--)
        av[i] = av[i-1];
    av[dest] = tmp;
}

static int __getopt_long_core(int argc, char *const *argv, const char *optstring, const struct option *longopts, int *idx, int longonly)
{
    optarg = 0;
    // 处理长选项
    if (longopts && argv[optind][0] == '-' &&
        ((longonly && argv[optind][1] && argv[optind][1] != '-') ||
         (argv[optind][1] == '-' && argv[optind][2])))
    {
        // 如果 optstring[0] 为 '+' 或 '-'，则判断 optstring[1] == ':'；否则，判断 optstring[0] == ':'。
        int colon = optstring[optstring[0]=='+'||optstring[0]=='-'] == ':';
        int i, cnt, match = -1;
        char *opt;
        // 遍历 longopts
        for (cnt = i = 0; longopts[i].name; i++) {
            const char *name = longopts[i].name;
            opt = argv[optind] + 1;
            // 处理长选项 `--xxx`
            if (*opt == '-') opt++;
            // 将长选项和 argv[optind]（去掉--后）的值进行比对
            for (; *name && *name == *opt; name++, opt++);
            // 如果依次比对完后，argv[optind] 后面仍有字符，且 后续字符不为 '='，则丢弃，进行下一次循环
            if (*opt && *opt != '=') continue;

            // 否则，匹配成功，并获取配的 longopts 索引。
            match = i;
            // 如果 name 为空，表示完全匹配成功，退出循环
            if (!*name) {
                cnt = 1;
                break;
            }
            // 否则 cnt++。
            cnt++;
        }
        // 匹配成功，则处理选项值
        if (cnt == 1) {
            i = match;
            // 将选项指针指向下一个
            optind++;
            // 将 optopt 设置成匹配的 longopt 的短选项值
            optopt = longopts[i].val;

            // 如果 argv[optind] 后续还有 '='，则判断对应 longopt 的设置，是否允许有值
            if (*opt == '=') {
                // 如果不允许有值，则报错处理
                if (!longopts[i].has_arg) {
                    if (colon || !opterr)
                        return '?';
                    __getopt_msg(argv[0], ": option does not take an argument: ", longopts[i].name, strlen(longopts[i].name));
                    return '?';
                }
                // 如果允许有值，则将值进行存储【重点】
                optarg = opt+1;

            // 如果对应的 longopt 设置要求有值
            } else if (longopts[i].has_arg == required_argument) {
                // 获取选项值，并进行判断
                // 如果选项值为空，则进行报错处理
                if (!(optarg = argv[optind])) {
                    if (colon) return ':';
                    if (!opterr) return '?';
                    __getopt_msg(argv[0], ": option requires an argument: ", longopts[i].name, strlen(longopts[i].name));
                    return '?';
                }
                // 如果选项值不为空，则选项指针指向下一个
                optind++;
            }
            //
            if (idx) *idx = i;
            // 如果 longopt 标志位有值，则将 flag 设置为 val
            if (longopts[i].flag) {
                *longopts[i].flag = longopts[i].val;
                return 0;
            }
            // 返回结果【重点】
            return longopts[i].val;
        }
        // 如果未匹配成功，且 选项的第二个字符为 '-'
        if (argv[optind][1] == '-') {
            // 报错处理
            if (!colon && opterr)
                __getopt_msg(argv[0], cnt ? ": option is ambiguous: " : ": unrecognized option: ", argv[optind]+2, strlen(argv[optind]+2));
            optind++;
            return '?';
        }
    }
    // 处理短选项
    return getopt(argc, argv, optstring);
}

static int __getopt_long(int argc, char *const *argv, const char *optstring, const struct option *longopts, int *idx, int longonly)
{
    int ret, skipped, resumed;
    // optind 初始值为 1，因为 argv[0] 是可执行文件的名称，argv[1..] 是传递给可执行文件的参数。
    // 如果 optind == 0 或 optreset == 1，则进行复位。
    if (!optind || optreset) {
        optreset = 0;
        __optpos = 0;
        optind = 1;
    }
    // 如果 optind 大于参数数量 或 参数列表对应位置无值，则返回 -1。
    if (optind >= argc || !argv[optind]) return -1;
    skipped = optind;
    // 如果 optstring[0] 不等于 '+' 且不等于 '-'，则遍历 argv，找到第一个参数，其第一个字符为 '-' 且第二个字符不为 NULL。
    // 本质上就是当遇到不合法选项时，查找第一个合法的选项。
    if (optstring[0] != '+' && optstring[0] != '-') {
        int i;
        for (i = optind; ; i++) {
            if (i >= argc || !argv[i]) return -1;
            if (argv[i][0] == '-' && argv[i][1]) break;
        }
        optind = i;
    }
    resumed = optind;
    // 上述内容初始化了 optind。
    //
    ret = __getopt_long_core(argc, argv, optstring, longopts, idx, longonly);
    // 经过 __getopt_long_core 的处理，optind 已经大于 resumed。
    // 如果中间存在不合法的选项时，则通过插入+后移，将不合法的选项进行后移
    if (resumed > skipped) {
        // 此时 optind 必然大于 skipped
        // cnt 表示 __getopt_long_core 处理对应数量的选项和参数
        int i, cnt = optind-resumed;
        for (i = 0; i < cnt; i++)
            permute(argv, skipped, optind-1);
        optind = skipped + cnt;
    }
    return ret;
}

// char *const *argv: 限制修改指针地址
// const char *optstring: 限制修改指针所指向的值
int getopt_long(int argc, char *const *argv, const char *optstring, const struct option *longopts, int *idx)
{
    return __getopt_long(argc, argv, optstring, longopts, idx, 0);
}

int getopt_long_only(int argc, char *const *argv, const char *optstring, const struct option *longopts, int *idx)
{
    return __getopt_long(argc, argv, optstring, longopts, idx, 1);
}
