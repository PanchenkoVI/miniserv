# include "MIMEs.hpp"

MIMEs::MIMEs() {
	_mime[".aac"] = "audio/aac";
	_mime[".abw"] = "application/x-abiword";
	_mime[".arc"] = "application/octet-stream";
	_mime[".avi"] = "video/x-msvideo";
	_mime[".azw"] = "application/vnd.amazon.ebook";
	_mime[".bin"] = "application/octet-stream";
	_mime[".bmp"] = "image/bmp";
	_mime[".bz"] = "application/x-bzip";
	_mime[".bz2"] = "application/x-bzip2";
	_mime[".csh"] = "application/x-csh";
	_mime[".css"] = "text/css";
	_mime[".csv"] = "text/csv";
	_mime[".doc"] = "application/msword";
	_mime[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	_mime[".eot"] = "application/vnd.ms-fontobject";
	_mime[".epub"] = "application/epub+zip";
	_mime[".gif"] = "image/gif";
	_mime[".htm"] = "text/html";
	_mime[".html"] = "text/html";
	_mime[".ico"] = "image/x-icon";
	_mime[".ics"] = "text/calendar";
	_mime[".jar"] = "application/java-archive";
	_mime[".jpeg"] = "image/jpeg";
	_mime[".jpg"] = "image/jpeg";
	_mime[".js"] = "application/javascript";
	_mime[".json"] = "application/json";
	_mime[".mid"] = "audio/midi";
	_mime[".midi"] = "audio/midi";
	_mime[".mpeg"] = "video/mpeg";
	_mime[".mpkg"] = "application/vnd.apple.installer+xml";
	_mime[".odp"] = "application/vnd.oasis.opendocument.presentation";
	_mime[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	_mime[".odt"] = "application/vnd.oasis.opendocument.text";
	_mime[".oga"] = "audio/ogg";
	_mime[".ogv"] = "video/ogg";
	_mime[".ogx"] = "application/ogg";
	_mime[".otf"] = "font/otf";
	_mime[".png"] = "image/png";
	_mime[".pdf"] = "application/pdf";
	_mime[".ppt"] = "application/vnd.ms-powerpoint";
	_mime[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	_mime[".rar"] = "application/x-rar-compressed";
	_mime[".rtf"] = "application/rtf";
	_mime[".sh"] = "application/x-sh";
	_mime[".svg"] = "image/svg+xml";
	_mime[".woff"] = "font/woff";
	_mime[".woff2"] = "font/woff2";
	_mime[".xhtml"] = "application/xhtml+xml";
	_mime[".xls"] = "application/vnd.ms-excel";
	_mime[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	_mime[".xml"] = "application/xml";
	_mime[".xul"] = "application/vnd.mozilla.xul+xml";
	_mime[".zip"] = "application/zip";
	_mime[".3gp"] = "audio/3gpp";
	_mime[".3g2"] = "audio/3gpp2";
	_mime[".7z"] = "application/x-7z-compressed";
	_mime["txt"] = "text/plain";
}

std::string MIMEs::getMime(std::string mime) {
	if (!mime.length()) { return "application/octet-stream"; }
	if (_mime.count(mime)) { return _mime[mime]; }
	return "application/octet-stream";
};
