dnl Define ourselves
AC_INIT(nanocap, 0.1)

dnl Canonicalize environments
AC_CANONICAL_BUILD
AC_CANONICAL_HOST

dnl Ensure our C compiler supports atleast C11
AC_PROG_CC
AC_PROG_CC_C11(, [
	AC_MSG_ERROR([C11 support required])
])

dnl Ensure our C++ compiler supports a new enough C++ standard
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX(14,, mandatory)

dnl Checks for programs.
AC_PROG_MAKE_SET
AC_PROG_INSTALL

dnl Look for SQLite3
AX_LIB_SQLITE3
AS_IF([test "${WANT_SQLITE3}" != 'yes'], [
	AC_MSG_ERROR([sqlite3 is required])
])
AS_IF([test -z "${SQLITE3_VERSION}"], [
	AC_MSG_ERROR([sqlite3 is required])
])

dnl Determine how to handle encodings for Kaitai
AC_CHECK_HEADER(iconv.h, [
	AC_DEFINE([KS_STR_ENCODING_ICONV], [1], [Define if you have iconv support])
], [
	AC_DEFINE([KS_STR_ENCODING_NONE], [1], [Define if you have have no localization support])
])

dnl Enable Boost
NANOCAP_FIND_BOOST

dnl Enable pcap++
PKG_CHECK_MODULES([PCAPPLUSPLUS], [PcapPlusPlus],, [
	AC_MSG_ERROR([pcap++ is required])
])

dnl Produce output
AC_OUTPUT(Makefile)
