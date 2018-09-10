#include <Rcpp.h>
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
    if ((cleaned.length() > 0) && (cleaned.at(cleaned.length()-1) == '.')) cleaned.pop_back();

    // lowercase it
    rc = psl_str_to_utf8lower(cleaned.c_str(), "utf-8", "en", &lower);

    if (rc == PSL_SUCCESS) {
      result = psl_registrable_domain(psl, lower);
      output[i] = (result) ? String(result) : NA_STRING;
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
    if ((cleaned.length() > 0) && (cleaned.at(cleaned.length()-1) == '.')) cleaned.pop_back();

    // lowercase it
    rc = psl_str_to_utf8lower(cleaned.c_str(), "utf-8", "en", &lower);

    if (rc == PSL_SUCCESS) {
      result = psl_unregistrable_domain(psl, lower);
      if (result) {
        std::string res(result);
        if ((res.length() > 0) && (res.at(0) == '.')) res.erase(0, 1);
        output[i] = res;
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
LogicalVector is_public_suffix(CharacterVector domains) {

  unsigned int input_size = domains.size();
  LogicalVector output(input_size);
  char *lower = NULL;
  int rc;
  const psl_ctx_t *psl = psl_builtin();

  for (unsigned int i = 0; i < input_size; i++) {

    // remove trailing period if any
    std::string cleaned = Rcpp::as<std::string>(domains[i]);
    if ((cleaned.length() > 0) && (cleaned.at(cleaned.length()-1) == '.')) cleaned.pop_back();

    // lowercase it
    rc = psl_str_to_utf8lower(cleaned.c_str(), "utf-8", "en", &lower);

    output[i] =(rc == PSL_SUCCESS) ? (psl_is_public_suffix(psl, lower) == 1) : NA_LOGICAL;

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
    if ((cleaned.length() > 0) && (cleaned.at(cleaned.length()-1) == '.')) cleaned.pop_back();

    // lowercase it
    rc = psl_str_to_utf8lower(cleaned.c_str(), "utf-8", "en", &lower);

    if (rc == PSL_SUCCESS) {

      // no dots at end and lowercased
      std::string normd = std::string(lower);
      normalized[i] = normd;

      // try to get the suffix
      result = psl_unregistrable_domain(psl, lower);
      std::string suf = std::string(result);
      if (result) {
        if ((suf.length() > 0) && (suf.at(0) == '.')) suf.erase(0, 1);
        suffix[i] = suf;
      } else {
        suffix[i] = NA_STRING;
      }

      // try to get the apex
      result = psl_registrable_domain(psl, lower);
      apex[i] = (result) ? String(result) : NA_STRING;

      if (result) {

        std::string apx = std::string(result);

        int suf_pos = apx.rfind(suf);
        std::string dom = apx.substr(0, suf_pos);

        int apex_pos = normd.rfind(apx);
        std::string subdom = (apex_pos == 0) ? "" : normd.substr(0, apex_pos);

        if ((dom.length() > 0) && (dom.at(dom.length()-1) == '.')) dom.pop_back();
        if ((subdom.length() > 0) && (subdom.at(subdom.length()-1) == '.')) subdom.pop_back();

        domain[i] = dom;
        subdomain[i] = subdom;

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

//' Separate a domain into component parts (`urltools` compatibility function)
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
  const char *result;
  const psl_ctx_t *psl = psl_builtin();

  for (unsigned int i = 0; i < input_size; i++) {

    std::string cleaned = Rcpp::as<std::string>(domains[i]);
    if ((cleaned.length() > 0) && (cleaned.at(cleaned.length()-1) == '.')) cleaned.pop_back();

    // lowercase it
    rc = psl_str_to_utf8lower(cleaned.c_str(), "utf-8", "en", &lower);

    if (rc == PSL_SUCCESS) {

      std::string normalized(lower);

      // try to get the suffix
      result = psl_unregistrable_domain(psl, lower);

      if (result) {

        std::string suf = std::string(result);
        if ((suf.length() > 0) && (suf.at(0) == '.')) suf.erase(0, 1);
        suffix[i] = suf;

        result = psl_registrable_domain(psl, lower);

        if (result) {

          std::string apex(result);

          int suf_pos = apex.rfind(suf);
          std::string dom = apex.substr(0, suf_pos);

          int apex_pos = normalized.rfind(apex);
          std::string subdom = (apex_pos == 0) ? "" : normalized.substr(0, apex_pos);

          if ((dom.length() > 0) && (dom.at(dom.length()-1) == '.')) dom.pop_back();
          if ((subdom.length() > 0) && (subdom.at(subdom.length()-1) == '.')) subdom.pop_back();

          domain[i] = (dom);
          subdomain[i] = (subdom);

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

  return(out);

}
