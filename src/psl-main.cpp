#include <Rcpp.h>
#include <regex>
#include <libpsl.h>

using namespace Rcpp;

//' Return the apex/top-private domain from a vector of domains
//'
//' @md
//' @param domains character vector of domains
//' @return character vector
//' @export
// [[Rcpp::export]]
CharacterVector apex_domain(CharacterVector domains) {

  unsigned int input_size = domains.size();
  CharacterVector output(input_size);
  char *lower = NULL;
  int rc;
  const char * result;
  const psl_ctx_t *psl = psl_builtin();

  for (unsigned int i = 0; i < input_size; i++) {

    // remove trailing period if any
    std::string cleaned = Rcpp::as<std::string>(domains[i]);
    if (cleaned.length() > 0) {
      if (cleaned.at(cleaned.length()-1) == '.') cleaned.pop_back();
    }

    // lowercase it
    rc = psl_str_to_utf8lower(
      cleaned.c_str(),
      "utf-8", "en",
      &lower
    );

    if (rc == PSL_SUCCESS) {
      result = psl_registrable_domain(psl, lower);
      if (result) {
        output[i] = std::string(result);
      } else {
        output[i] = NA_STRING;
      }
    } else {
      output[i] = NA_STRING;
    }

    psl_free_string(lower);

  }

  return(output);

}

//' Return the public suffix from a vector of domains
//'
//' @md
//' @param domains character vector of domains
//' @return character vector
//' @export
// [[Rcpp::export]]
CharacterVector public_suffix(CharacterVector domains) {

  unsigned int input_size = domains.size();
  CharacterVector output(input_size);
  char *lower = NULL;
  int rc;
  const char * result;
  const psl_ctx_t *psl = psl_builtin();

  for (unsigned int i = 0; i < input_size; i++) {

    // remove trailing period if any
    std::string cleaned = Rcpp::as<std::string>(domains[i]);
    if (cleaned.length() > 0) {
      if (cleaned.at(cleaned.length()-1) == '.') cleaned.pop_back();
    }

    // lowercase it
    rc = psl_str_to_utf8lower(
      cleaned.c_str(),
      "utf-8", "en",
      &lower
    );

    if (rc == PSL_SUCCESS) {
      result = psl_unregistrable_domain(psl, lower);
      if (result) {
        output[i] = std::string(result);
      } else {
        output[i] = NA_STRING;
      }
    } else {
      output[i] = NA_STRING;
    }

    psl_free_string(lower);

  }

  return(output);

}

//' Test whether a domain is a public suffix
//'
//' @md
//' @param domains character vector of domains
//' @return character vector
//' @export
// [[Rcpp::export]]
std::vector< bool > is_public_suffix(CharacterVector domains) {

  unsigned int input_size = domains.size();
  std::vector < bool > output(input_size);
  char *lower = NULL;
  int rc;
  const psl_ctx_t *psl = psl_builtin();

  for (unsigned int i = 0; i < input_size; i++) {

    // remove trailing period if any
    std::string cleaned = Rcpp::as<std::string>(domains[i]);
    if (cleaned.length() > 0) {
      if (cleaned.at(cleaned.length()-1) == '.') cleaned.pop_back();
    }

    // lowercase it
    rc = psl_str_to_utf8lower(
      cleaned.c_str(),
      "utf-8", "en",
      &lower
    );

    if (rc == PSL_SUCCESS) {
      output[i] = (psl_is_public_suffix(psl, lower) == 1);
    } else {
      output[i] = NA_LOGICAL;
    }

    psl_free_string(lower);

  }

  return(output);

}

//' Separate a domain into component parts
//'
//' @md
//' @param domains character vector of domains
//' @return data frame
//' @export
// [[Rcpp::export]]
DataFrame suffix_extract(CharacterVector domains) {

  unsigned int input_size = domains.size();

  CharacterVector normalized(input_size);
  CharacterVector subdomain(input_size);
  CharacterVector apex(input_size);
  CharacterVector domain(input_size);
  CharacterVector suffix(input_size);

  char *lower = NULL;
  int rc;
  const char * result;
  const psl_ctx_t *psl = psl_builtin();

  for (unsigned int i = 0; i < input_size; i++) {

    // remove trailing period if any
    std::string cleaned = Rcpp::as<std::string>(domains[i]);
    if (cleaned.length() > 0) {
      if (cleaned.at(cleaned.length()-1) == '.') cleaned.pop_back();
    }

    // lowercase it
    rc = psl_str_to_utf8lower(
      cleaned.c_str(),
      "utf-8", "en",
      &lower
    );

    if (rc == PSL_SUCCESS) {

      // no dots at end and lowercased
      normalized[i] = std::string(lower);

      // try to get the suffix
      result = psl_unregistrable_domain(psl, lower);
      if (result) {
        suffix[i] = std::string(result);
      } else {
        suffix[i] = NA_STRING;
      }

      // try to get the apex
      result = psl_registrable_domain(psl, lower);
      if (result) {
        apex[i] = std::string(result);
      } else {
        apex[i] = NA_STRING;
      }

      if ((suffix[i] != NA_STRING) && (apex[i] != NA_STRING)) {

        std::regex trail_suf("[\\.]*" + Rcpp::as<std::string>(suffix[i]) + "$");
        domain[i] = std::regex_replace(
          Rcpp::as<std::string>(apex[i]),
          trail_suf, ""
        );

        std::regex apex_suf("[\\.]*" + Rcpp::as<std::string>(apex[i]) + "$");
        subdomain[i] = std::regex_replace(
          Rcpp::as<std::string>(normalized[i]),
          apex_suf, ""
        );

      } else {
        domain[i] = NA_STRING;
        subdomain[i] = NA_STRING;
      }

    } else {
      normalized[i] = NA_STRING;
      subdomain[i] = NA_STRING;
      apex[i] = NA_STRING;
      domain[i] = NA_STRING;
      suffix[i] = NA_STRING;
    }

    psl_free_string(lower);

  }

  DataFrame out = DataFrame::create(
    _["orig"] = domains,
    _["normalized"] = normalized,
    _["subdomain"] = subdomain,
    _["apex"] = apex,
    _["domain"] = domain,
    _["suffix"] = suffix,
    _["stringsAsFactors"] = false
  );

  out.attr("class") = CharacterVector::create("tbl_df", "tbl", "data.frame");

  return(out);

}

//' Separate a domain into component parts
//'
//' Compatibility function for those using `urltools::suffix_extract()`
//'
//' @md
//' @param domains character vector of domains
//' @return data frame
//' @export
// [[Rcpp::export]]
DataFrame suffix_extract2(CharacterVector domains) {

  unsigned int input_size = domains.size();

  CharacterVector subdomain(input_size);
  CharacterVector domain(input_size);
  CharacterVector suffix(input_size);

  char *lower = NULL;
  int rc;
  const char * result;
  const psl_ctx_t *psl = psl_builtin();

  for (unsigned int i = 0; i < input_size; i++) {

    std::string cleaned = Rcpp::as<std::string>(domains[i]);
    if (cleaned.length() > 0) {
      if (cleaned.at(cleaned.length()-1) == '.') cleaned.pop_back();
    }

    // lowercase it
    rc = psl_str_to_utf8lower(
      cleaned.c_str(),
      "utf-8", "en",
      &lower
    );

    if (rc == PSL_SUCCESS) {

      std::string normalized(lower);

      // try to get the suffix
      result = psl_unregistrable_domain(psl, lower);

      if (result) {

        std::string suf = std::string(result);
        suffix[i] = suf;

        result = psl_registrable_domain(psl, lower);

        if (result) {

          std::string apex(result);

          std::regex trail_suf("[\\.]*" + suf + "$");
          std::regex apex_suf("[\\.]*" + apex + "$");

          domain[i] = std::regex_replace(apex, trail_suf, "");
          subdomain[i] = std::regex_replace(normalized, apex_suf, "");

        } else {
          subdomain[i] = NA_STRING;
          domain[i] = NA_STRING;
        }
      } else {
        subdomain[i] = NA_STRING;
        suffix[i] = NA_STRING;
        domain[i] = NA_STRING;
      }

    } else {
      subdomain[i] = NA_STRING;
      domain[i] = NA_STRING;
      suffix[i] = NA_STRING;
    }

    psl_free_string(lower);

  }

  DataFrame out = DataFrame::create(
    _["host"] = domains,
    _["subdomain"] = subdomain,
    _["domain"] = domain,
    _["suffix"] = suffix,
    _["stringsAsFactors"] = false
  );

  out.attr("class") = CharacterVector::create("tbl_df", "tbl", "data.frame");

  return(out);

}
