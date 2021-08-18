#include "Utils.hpp"

namespace ft {
	static void	*ft_calloc(size_t count, size_t size) {
		void	*p;
		if (!(p = malloc(count * size))) { return (NULL); }
		memset(p, 0, count * size);
		return p;
	}

	static char	*ft_strjoin(char const *s1, char const *s2) {
		char	*str;
		if (!(str = (char*)ft_calloc(strlen(s1) + strlen(s2) + 1, sizeof(char)))) { return 0; }
		strcpy(str, s1);
		strcat(str, s2);
		return str;
	}

	static size_t		ft_strlen(char *s, char c) {
		size_t	_;
		_ = 0;
		while (s[_] and s[_] != c) { _++; }
		return _;
	}

	static char	*ft_substr(char const *s, unsigned int start, size_t len) {
		char	*str;
		int		i;

		i = 0;
		if (!s) { return 0; }
		if (start > strlen(s)) { return 0; }
		if (!(str = (char*)ft_calloc(len + 1, sizeof(char)))) { return 0; }
		while (s[start] and len) {
			str[i] = s[start];
			i++;
			start++;
			len--;
		}
		str[i] = '\0';
		return str;
	}

	static int		ft_scount(char c, char *str) {
		int		i;
		i = 0;
		while (str[i]) 	{
			if (str[i] == c) { return (i); }
			i++;
		}
		return -1;
	}

	static int	getLine(char *s[], int fd) {
		char	*tmp;
		if (ft_scount('\n', s[fd]) >= 0) {
			tmp = s[fd];
			s[fd] = ft_substr(s[fd], ft_scount('\n', s[fd]) + 1, strlen(s[fd]));
			free(tmp);
		}
		else if (strlen(s[fd]) > 0) { free(s[fd]); s[fd] = 0; }
		else { free(s[fd]); s[fd] = 0; return 0; }
		return 1;
	}

	int		gnl(int fd, char **line)
	{
		int				ret;
		char			*tmp;
		char			buf[128 + 1];
		static char		*s[10240];

		if (fd < 0 or !line or 128 < 1 or read(fd, buf, 0) < 0) { return -1; }
		if (!s[fd] and !(s[fd] = (char*)ft_calloc(1, sizeof(char *)))) { return -1; }
		while ((ft_scount('\n', s[fd])) < 0 and (ret = read(fd, buf, 128)) > 0) {
			buf[ret] = '\0';
			tmp = s[fd];
			s[fd] = ft_strjoin(s[fd], buf);
			free(tmp);
		}
		if (s[fd]) { *line = ft_substr(s[fd], 0, ft_strlen(s[fd], '\n')); }
		if (!getLine(s, fd)) { return 0; }
		return 1;
	}


	std::string stol(std::string s) {
		std::transform(s.begin(), s.end(), s.begin(), ::tolower);
		return s;
	}

	std::string stoup(std::string s) {
		std::transform(s.begin(), s.end(), s.begin(), ::toupper);
		return s;
	}

	std::string sltrim(std::string str, char c) {
		size_t pos = str.find_first_not_of(c);
		if (pos != size_t(-1)) { str.erase(0, pos); }
		return str;
	}

	std::string srtrim(std::string str, char c) {
		size_t pos = str.find_last_not_of(c);
		if (pos == size_t(-1)) { return ""; }
		str.erase(pos + 1, str.length());
		return str;
	}

	size_t stoh(std::string &str) {
		std::stringstream ss;
		size_t hex;
		ss << std::hex << str;
		ss >> hex;
		return hex;
	}

	char *strdup(const char *str) {
		if (!str) { return 0; }
		char *s = (char*)malloc(sizeof(char) * (strlen(str) + 1));
		if (!s) { return 0; }
		size_t i = 0;
		while (str[i]) { s[i] = str[i]; i++; }
		s[i] = '\0';
		return s;
	}

	std::string uchar(std::string str) {
		std::string::iterator it = str.begin();
		while (it != str.end()) {
			if (*it == '/') {
				it++;
				while (it != str.end() and *it == '/') { it = str.erase(it); }
			}
			else { it++; }
		}
		return str;
	}

	int stoi(const std::string &str, std::size_t *pos, int base) {
		static std::string b("0123456789abcdef");
		(void)pos;
		std::string ob(b, 0, base);
		int i = 0;
		if (str.empty()) { throw std::invalid_argument("invalid_argument"); }
		bool sign = (str[0] != '-');
		if (str[i] == '+' or str[i] == '-') { i++; }
		long result = 0;
		while (str[i]) {
			if (ob.find(str[i]) == size_t(-1)) { throw std::invalid_argument("invalid_argument"); }
			result = result * base + ob.find(str[i]);
			if (result > std::numeric_limits<int>::max()) { throw std::invalid_argument("invalid_argument"); }
			i++;
		}
		return sign ? result : -result;
	}
};
