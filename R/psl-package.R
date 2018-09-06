#' Extract Internet Domain Components Using the Public Suffix List
#'
#' The 'Public Suffix List' (<https://publicsuffix.org/>) is a collection
#' of top-level domains ('TLDs') which include global top-level domainsa ('gTLDs')
#' such as '.com' and '.net'; country top-level domains ('ccTLDs') such as '.de' and
#' '.cn'; and, brand top-level domains such as '.apple' and '.google'. Tools are provided
#' to extract internet domain components using the public suffix list base data.
#'
#' - `libpsl`: <https://github.com/rockdaboot/libpsl>
#' - Public Suffix List: <https://publicsuffix.org/>
#'
#' - URL: <https://gitlab.com/hrbrmstr/psl>
#' - BugReports: <https://gitlab.com/hrbrmstr/psl/issues>
#'
#' @md
#' @name psl
#' @docType package
#' @author Bob Rudis (bob@@rud.is)
#' @useDynLib psl
#' @importFrom Rcpp sourceCpp
NULL