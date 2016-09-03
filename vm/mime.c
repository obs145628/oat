#include "mime.h"
#include <string.h>
#include "str.h"

#define NB_MIMES 434

static const char* exts_[] = {
   "3dm",
   "3dmf",
   "a",
   "aab",
   "aam",
   "aas",
   "abc",
   "acgi",
   "afl",
   "ai",
   "aif",
   "aifc",
   "aiff",
   "aim",
   "aip",
   "ani",
   "aos",
   "aps",
   "arc",
   "arj",
   "art",
   "asf",
   "asm",
   "asp",
   "asx",
   "au",
   "avi",
   "avs",
   "bcpio",
   "bin",
   "bm",
   "bmp",
   "boo",
   "book",
   "boz",
   "bsh",
   "bz",
   "bz2",
   "c",
   "c++",
   "cat",
   "cc",
   "ccad",
   "cco",
   "cdf",
   "cer",
   "cha",
   "chat",
   "class",
   "com",
   "conf",
   "cpio",
   "cpp",
   "cpt",
   "crl",
   "crt",
   "csh",
   "css",
   "cxx",
   "dcr",
   "deepv",
   "def",
   "der",
   "dir",
   "dl",
   "doc",
   "dot",
   "dp",
   "drw",
   "dump",
   "dv",
   "dvi",
   "dwf",
   "dwg",
   "dxf",
   "el",
   "elc",
   "env",
   "eps",
   "es",
   "etx",
   "evy",
   "exe",
   "f",
   "f77",
   "f90",
   "fdf",
   "fif",
   "fli",
   "flo",
   "flx",
   "fmf",
   "for",
   "fpx",
   "frl",
   "funk",
   "g",
   "g3",
   "gif",
   "gl",
   "gsd",
   "gsm",
   "gsp",
   "gss",
   "gtar",
   "gz",
   "gzip",
   "h",
   "hdf",
   "help",
   "hgl",
   "hh",
   "hlb",
   "hlp",
   "hpg",
   "hpgl",
   "hqx",
   "hta",
   "htc",
   "htm",
   "html",
   "htmls",
   "htt",
   "htx",
   "ice",
   "ico",
   "idc",
   "ief",
   "iefs",
   "iges",
   "igs",
   "ima",
   "imap",
   "inf",
   "ins",
   "ip",
   "isu",
   "it",
   "iv",
   "ivr",
   "ivy",
   "jam",
   "jav",
   "java",
   "jcm",
   "jfif",
   "jfif-tbnl",
   "jpe",
   "jpeg",
   "jpg",
   "jps",
   "js",
   "jut",
   "kar",
   "ksh",
   "la",
   "lam",
   "latex",
   "lha",
   "lhx",
   "list",
   "lma",
   "log",
   "lsp",
   "lst",
   "lsx",
   "ltx",
   "lzh",
   "lzx",
   "m",
   "m1v",
   "m2a",
   "m2v",
   "m3u",
   "man",
   "map",
   "mar",
   "mdb",
   "mcs$",
   "mcd",
   "mcf",
   "mcp",
   "me",
   "mht",
   "mhtml",
   "mid",
   "mif",
   "mjpg",
   "mm",
   "mme",
   "mod",
   "moov",
   "mov",
   "movie",
   "mp2",
   "mp3",
   "mpa",
   "mpc",
   "mpe",
   "mpeg",
   "mpg",
   "mpga",
   "mpp",
   "mpt",
   "mpv",
   "mpx",
   "mrc",
   "ms",
   "mv",
   "my",
   "mzz",
   "nap",
   "naplps",
   "nc",
   "ncm",
   "nif",
   "niff",
   "nix",
   "nsc",
   "nvd",
   "o",
   "oda",
   "omc",
   "omcd",
   "omcr",
   "p",
   "p10",
   "p12",
   "p7a",
   "p7c",
   "p7m",
   "p7r",
   "p7s",
   "part",
   "pas",
   "pbm",
   "pcl",
   "pct",
   "pcx",
   "pdb",
   "pdf",
   "pfunk",
   "pgm",
   "pic",
   "pict",
   "pkg",
   "pko",
   "pl",
   "plx",
   "pm",
   "pm4",
   "pm5",
   "png",
   "pnm",
   "pot",
   "pov",
   "ppa",
   "ppm",
   "pps",
   "ppt",
   "ppz",
   "pre",
   "prt",
   "ps",
   "psd",
   "pvu",
   "pwz",
   "py",
   "pyc",
   "qcp",
   "qd3",
   "qd3d",
   "qif",
   "qt",
   "qtc",
   "qti",
   "qtif",
   "ra",
   "ram",
   "ras",
   "rast",
   "rexx",
   "rf",
   "rgb",
   "rm",
   "rmi",
   "rmn",
   "rmp",
   "rng",
   "rnx",
   "roff",
   "rp",
   "rpm",
   "rt",
   "rtf",
   "rv",
   "s",
   "s3m",
   "saveme",
   "sbk",
   "scm",
   "sdml",
   "sdp",
   "sdr",
   "sea",
   "set",
   "sgm",
   "sgml",
   "sh",
   "shar",
   "shtml",
   "sid",
   "sit",
   "skd",
   "skt",
   "sl",
   "smi",
   "smil",
   "snd",
   "sol",
   "spc",
   "spl",
   "spr",
   "sprite",
   "src",
   "ssi",
   "ssm",
   "sst",
   "step",
   "stl",
   "stp",
   "sv4cpio",
   "sv4crc",
   "svf",
   "svr",
   "swf",
   "t",
   "talk",
   "tar",
   "tbk",
   "tcl",
   "tcsh",
   "tex",
   "texi",
   "texinfo",
   "text",
   "tgz",
   "tif",
   "tr",
   "tsi",
   "tsp",
   "tsv",
   "turbot",
   "txt",
   "uil",
   "uni",
   "unis",
   "unv",
   "uri",
   "uris",
   "ustar",
   "uu",
   "uue",
   "vcd",
   "vcs",
   "vda",
   "vdo",
   "vew",
   "viv",
   "vivo",
   "vmd",
   "voc",
   "vos",
   "vox",
   "vqe",
   "vqf",
   "vql",
   "vrml",
   "vrt",
   "vsd",
   "vst",
   "vsw",
   "w60",
   "w61",
   "w6w",
   "wav",
   "wb1",
   "wbmp",
   "web",
   "wiz",
   "wk1",
   "wmf",
   "wml",
   "wmlc",
   "wmls",
   "wmlsc",
   "word",
   "wp",
   "wp5",
   "wp6",
   "wpd",
   "wq1",
   "wri",
   "wrl",
   "wrz",
   "wsc",
   "wtk",
   "xbm",
   "xdr",
   "xgz",
   "xif",
   "xla",
   "xlb",
   "xlc",
   "xld",
   "xlk",
   "xll",
   "xlm",
   "xls",
   "xlt",
   "xlv",
   "xlw",
   "xm",
   "xml",
   "xmz",
   "xpix",
   "xmp",
   "xsr",
   "xwd",
   "xyz",
   "z",
   "zip",
   "zoo",
   "zsh"
};

static const char* types_[] = {
   "x-world/x-3dmf",
   "x-world/x-3dmf",
   "application/octet-stream",
   "application/x-authorware-bin",
   "application/x-authorware-map",
   "application/x-authorware-seg",
   "text/vnd.abc",
   "text/html",
   "video/animaflex",
   "application/postscript",
   "audio/aiff",
   "audio/aiff",
   "audio/aiff",
   "application/x-aim",
   "text/x-audiosoft-intra",
   "application/x-navi-animation",
   "application/x-nokia-9000-communicator-add-on-software",
   "application/mime",
   "application/octet-stream",
   "application/octet-stream",
   "image/x-jg",
   "video/x-ms-asf",
   "text/x-asm",
   "text/asp",
   "application/x-mplayer2",
   "audio/basic",
   "video/avi",
   "video/avs-video",
   "application/x-bcpio",
   "application/octet-stream",
   "image/bmp",
   "image/bmp",
   "application/book",
   "application/book",
   "application/x-bzip2",
   "application/x-bsh",
   "application/x-bzip",
   "application/x-bzip2",
   "text/plain",
   "text/plain",
   "application/vnd.ms-pki.seccat",
   "text/plain",
   "application/clariscad",
   "application/x-cocoa",
   "application/cdf",
   "application/pkix-cert",
   "application/x-chat",
   "application/x-chat",
   "application/java-byte-code",
   "application/octet-stream",
   "text/plain",
   "application/x-cpio",
   "text/x-c",
   "application/mac-compactpro",
   "application/pkcs-crl",
   "application/pkix-cert",
   "application/x-csh",
   "text/css",
   "text/plain",
   "application/x-director",
   "application/x-deepv",
   "text/plain",
   "application/x-x509-ca-cert",
   "application/x-director",
   "video/dl",
   "application/msword",
   "application/msword",
   "application/commonground",
   "application/drafting",
   "application/octet-stream",
   "video/x-dv",
   "video/x-dvi",
   "drawing/x-dwf",
   "application/acad",
   "application/dxf",
   "text/x-script.elisp",
   "application/x-elc",
   "application/x-envoy",
   "application/postscript",
   "application/x-esrehber",
   "text/x-setext",
   "application/envoy",
   "application/octet-stream",
   "text/x-fortran",
   "text/x-fortran",
   "text/x-fortran",
   "application/vnd.fdf",
   "application/fractals",
   "video/fli",
   "image/florian",
   "text/vnd.fmi.flexstor",
   "video/x-atomic3d-feature",
   "text/x-fortran",
   "image/vnd.fpx",
   "application/freeloader",
   "audio/make",
   "text/plain",
   "image/g3fax",
   "image/gif",
   "video/gl",
   "audio/x-gsm",
   "audio/x-gsm",
   "application/x-gsp",
   "application/x-gss",
   "application/x-gtar",
   "application/x-gzip",
   "application/x-gzip",
   "text/x-h",
   "application/x-hdf",
   "application/x-helpfile",
   "application/vnd.hp-hpgl",
   "text/x-h",
   "text/x-script",
   "application/hlp",
   "application/vnd.hp-hpgl",
   "application/vnd.hp-hpgl",
   "application/binhex",
   "application/hta",
   "text/x-component",
   "text/html",
   "text/html",
   "text/html",
   "text/webviewhtml",
   "text/html",
   "x-conference/x-cooltalk",
   "image/x-icon",
   "text/plain",
   "image/ief",
   "image/iefB",
   "application/iges",
   "application/iges",
   "application/x-ima",
   "application/x-httpd-imap",
   "application/inf",
   "application/x-internett-signup",
   "application/x-ip2",
   "video/x-isvideo",
   "audio/it",
   "application/x-inventor",
   "i-world/i-vrml",
   "application/x-livescreen",
   "audio/x-jam",
   "text/plain",
   "text/x-java-source",
   "application/x-java-commerce",
   "image/jpeg",
   "image/jpeg",
   "image/jpeg",
   "image/jpeg",
   "image/jpeg",
   "image/x-jps",
   "application/javascript",
   "image/jutvision",
   "audio/midi",
   "application/x-ksh",
   "audio/nspaudio",
   "audio/x-liveaudio",
   "application/x-latex",
   "application/lha",
   "application/octet-stream",
   "text/plain",
   "audio/nspaudio",
   "text/plain",
   "application/x-lisp",
   "text/plain",
   "text/x-la-asf",
   "application/x-latex",
   "application/octet-stream",
   "application/lzx",
   "text/x-m",
   "video/mpeg",
   "audio/mpeg",
   "video/mpeg",
   "audio/x-mpequrl",
   "application/x-troff-man",
   "application/x-navimap",
   "text/plain",
   "application/mbedlet",
   "application/x-magic-cap-package-1.0",
   "application/mcad",
   "image/vasa",
   "application/netmc",
   "application/x-troff-me",
   "message/rfc822",
   "message/rfc822",
   "audio/midi",
   "application/x-frame",
   "video/x-motion-jpeg",
   "application/base64",
   "application/base64",
   "audio/mod",
   "video/quicktime",
   "video/quicktime",
   "video/x-sgi-movie",
   "audio/mpeg",
   "audio/mpeg3",
   "audio/mpeg",
   "application/x-project",
   "video/mpeg",
   "video/mpeg",
   "video/mpeg",
   "audio/mpeg",
   "application/vnd.ms-project",
   "application/x-project",
   "application/x-project",
   "application/x-project",
   "application/marc",
   "application/x-troff-ms",
   "video/x-sgi-movie",
   "audio/make",
   "application/x-vnd.audioexplosion.mzz",
   "image/naplps",
   "image/naplps",
   "application/x-netcdf",
   "application/vnd.nokia.configuration-message",
   "image/x-niff",
   "image/x-niff",
   "application/x-mix-transfer",
   "application/x-conference",
   "application/x-navidoc",
   "application/octet-stream",
   "application/oda",
   "application/x-omc",
   "application/x-omcdatamaker",
   "application/x-omcregerator",
   "text/x-pascal",
   "application/pkcs10",
   "application/pkcs12",
   "application/x-pkcs7-signature",
   "application/pkcs7-mime",
   "application/pkcs7-mime",
   "application/x-pkcs7-certreqresp",
   "application/pkcs7-signature",
   "application/pro_eng",
   "text/pascal",
   "image/x-portable-bitmap",
   "application/vnd.hp-pcl",
   "image/x-pict",
   "image/x-pcx",
   "chemical/x-pdb",
   "application/pdf",
   "audio/make",
   "image/x-portable-graymap",
   "image/pict",
   "image/pict",
   "application/x-newton-compatible-pkg",
   "application/vnd.ms-pki.pko",
   "text/x-script.perl",
   "application/x-pixclscript",
   "image/x-xpixmap",
   "application/x-pagemaker",
   "application/x-pagemaker",
   "image/png",
   "application/x-portable-anymap",
   "application/mspowerpoint",
   "model/x-pov",
   "application/vnd.ms-powerpoint",
   "image/x-portable-pixmap",
   "application/mspowerpoint",
   "application/mspowerpoint",
   "application/mspowerpoint",
   "application/x-freelance",
   "application/pro_eng",
   "application/postscript",
   "application/octet-stream",
   "paleovu/x-pv",
   "application/vnd.ms-powerpoint",
   "text/x-script.phyton",
   "application/x-bytecode.python",
   "audio/vnd.qcelp",
   "x-world/x-3dmf",
   "x-world/x-3dmf",
   "image/x-quicktime",
   "video/quicktime",
   "video/x-qtc",
   "image/x-quicktime",
   "image/x-quicktime",
   "audio/x-pn-realaudio",
   "audio/x-pn-realaudio",
   "application/x-cmu-raster",
   "application/x-cmu-raster",
   "text/x-script.rexx",
   "image/vnd.rn-realflash",
   "image/x-rgb",
   "application/vnd.rn-realmedia",
   "audio/x-pn-realaudio",
   "audio/mid",
   "audio/x-pn-realaudio",
   "application/ringing-tones",
   "application/vnd.rn-realplayer",
   "application/x-troff",
   "image/vnd.rn-realpix",
   "audio/x-pn-realaudio-plugin",
   "text/richtext",
   "application/rtf",
   "video/vnd.rn-realvideo",
   "text/x-asm",
   "audio/s3m",
   "application/octet-stream",
   "application/x-tbook",
   "application/x-lotusscreencam",
   "text/plain",
   "application/sdp",
   "application/sounder",
   "application/sea",
   "application/setB",
   "text/sgml",
   "text/sgml",
   "application/x-bsh",
   "application/x-bsh",
   "text/html",
   "audio/x-psid",
   "audio/x-sit",
   "application/x-koan",
   "application/x-koan",
   "application/x-seelogo",
   "application/smil",
   "application/smil",
   "audio/basic",
   "application/solids",
   "application/x-pkcs7-certificates",
   "application/futuresplash",
   "application/x-sprite",
   "application/x-sprite",
   "application/x-wais-source",
   "text/x-server-parsed-html",
   "application/streamingmedia",
   "application/vnd.ms-pki.certstore",
   "application/step",
   "application/sla",
   "application/step",
   "application/x-sv4cpio",
   "application/x-sv4crc",
   "image/vnd.dwg",
   "application/x-world",
   "application/x-shockwave-flash",
   "application/x-troff",
   "text/x-speech",
   "application/x-tar",
   "application/toolbook",
   "application/x-tcl",
   "text/x-script.tcsh",
   "application/x-tex",
   "application/x-texinfo",
   "application/x-texinfo",
   "text/plain",
   "application/gnutar",
   "image/tiff",
   "application/x-troff",
   "audio/tsp-audio",
   "application/dsptype",
   "text/tab-separated-values",
   "image/florian",
   "text/plain",
   "text/x-uil",
   "text/uri-list",
   "text/uri-list",
   "application/i-deas",
   "text/uri-list",
   "text/uri-list",
   "application/x-ustar",
   "text/x-uuencode",
   "text/x-uuencode",
   "application/x-cdlink",
   "text/x-vcalendar",
   "application/vda",
   "video/vdo",
   "application/groupwise",
   "video/vivo",
   "video/vivo",
   "application/vocaltec-media-desc",
   "audio/vocB",
   "video/vosaic",
   "audio/voxware",
   "audio/x-twinvq-plugin",
   "audio/x-twinvq",
   "audio/x-twinvq-plugin",
   "application/x-vrml",
   "x-world/x-vrt",
   "application/x-visio",
   "application/x-visio",
   "application/x-visio",
   "application/wordperfect6.0",
   "application/wordperfect6.1",
   "application/msword",
   "audio/wav",
   "application/x-qpro",
   "image/vnd.wap.wbmp",
   "application/vnd.xara",
   "application/msword",
   "application/x-123",
   "windows/metafile",
   "text/vnd.wap.wml",
   "application/vnd.wap.wmlc",
   "text/vnd.wap.wmlscript",
   "application/vnd.wap.wmlscriptc",
   "application/msword",
   "application/wordperfectB",
   "application/wordperfect",
   "application/wordperfect",
   "application/wordperfect",
   "application/x-lotus",
   "application/mswrite",
   "model/vrml",
   "model/vrml",
   "text/scriplet",
   "application/x-wintalk",
   "image/x-xbitmap",
   "video/x-amt-demorun",
   "xgl/drawing",
   "image/vnd.xiff",
   "application/excel",
   "application/excel",
   "application/excel",
   "application/excel",
   "application/excel",
   "application/excel",
   "application/excel",
   "application/excel",
   "application/excel",
   "application/excel",
   "application/excel",
   "audio/xm",
   "application/xml",
   "xgl/movie",
   "application/x-vnd.ls-xpix",
   "image/x-xpixmap",
   "video/x-amt-showrun",
   "image/x-xwd",
   "chemical/x-pdb",
   "application/x-compressed",
   "application/x-compressed",
   "application/octet-stream",
   "text/x-script.zsh"
};


const char* mimetype_get(const char* str)
{
   if(*str == '.')
      ++str;
   if(*str == '\0')
      return NULL;

   int left = 0;
   int right = NB_MIMES - 1;

   while(left <= right)
   {
      int middle = (left + right) / 2;
      int diff = strCmpIgnoreCase(str, exts_[middle]);
      if(diff < 0)
         right = middle - 1;
      else if(diff > 0)
         left = middle + 1;
      else
         return types_[middle];
   }

   return NULL;
}