/*
============================================================================
Coral: COpy-numbeR ALterations
============================================================================
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
============================================================================
Contact: Tobias Rausch (rausch@embl.de)
============================================================================
*/

#ifndef BED_H
#define BED_H

#include <boost/filesystem.hpp>
#include <boost/multi_array.hpp>
#include <boost/progress.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/unordered_map.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <boost/math/distributions/chi_squared.hpp>
#include <boost/math/distributions/hypergeometric.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filter/zlib.hpp>

#include <htslib/faidx.h>
#include <htslib/vcf.h>
#include <htslib/sam.h>

#include "matrix.h"
#include "gflars.h"

namespace coralns
{

  // Flattens overlapping intervals
  template<typename TRegionsGenome>
  inline int32_t
    _parseBedIntervals(std::string const& filename, bool const filePresent, bam_hdr_t* hdr, TRegionsGenome& bedRegions) {
    typedef typename TRegionsGenome::value_type TChrIntervals;
    typedef typename TChrIntervals::interval_type TIVal;

    int32_t intervals = 0;
    if (filePresent) {
      bedRegions.resize(hdr->n_targets, TChrIntervals());
      std::ifstream chrFile(filename.c_str(), std::ifstream::in);
      if (chrFile.is_open()) {
	while (chrFile.good()) {
	  std::string chrFromFile;
	  getline(chrFile, chrFromFile);
	  typedef boost::tokenizer< boost::char_separator<char> > Tokenizer;
	  boost::char_separator<char> sep(" \t,;");
	  Tokenizer tokens(chrFromFile, sep);
	  Tokenizer::iterator tokIter = tokens.begin();
	  if (tokIter!=tokens.end()) {
	    std::string chrName = *tokIter++;
	    int32_t tid = bam_name2id(hdr, chrName.c_str());
	    if (tid >= 0) {
	      if (tokIter!=tokens.end()) {
		int32_t start = boost::lexical_cast<int32_t>(*tokIter++);
		int32_t end = boost::lexical_cast<int32_t>(*tokIter++);
		bedRegions[tid].insert(TIVal::right_open(start, end));
		++intervals;
	      }
	    }
	  }
	}
      }
      chrFile.close();
    }
    return intervals;
  }


  // Keeps overlapping intervals
  template<typename TRegionsGenome>
  inline int32_t
  _parsePotOverlappingIntervals(std::string const& filename, bool const filePresent, bam_hdr_t* hdr, TRegionsGenome& bedRegions) {
    typedef typename TRegionsGenome::value_type TChrIntervals;
	
    int32_t intervals = 0;
    if (filePresent) {
      bedRegions.resize(hdr->n_targets, TChrIntervals());
      std::ifstream chrFile(filename.c_str(), std::ifstream::in);
      if (chrFile.is_open()) {
	while (chrFile.good()) {
	  std::string chrFromFile;
	  getline(chrFile, chrFromFile);
	  typedef boost::tokenizer< boost::char_separator<char> > Tokenizer;
	  boost::char_separator<char> sep(" \t,;");
	  Tokenizer tokens(chrFromFile, sep);
	  Tokenizer::iterator tokIter = tokens.begin();
	  if (tokIter!=tokens.end()) {
	    std::string chrName = *tokIter++;
	    int32_t tid = bam_name2id(hdr, chrName.c_str());
	    if (tid >= 0) {
	      if (tokIter!=tokens.end()) {
		int32_t start = boost::lexical_cast<int32_t>(*tokIter++);
		int32_t end = boost::lexical_cast<int32_t>(*tokIter++);
		bedRegions[tid].insert(std::make_pair(start, end));
		++intervals;
	      }
	    }
	  }
	}
      }
      chrFile.close();
    }
    return intervals;
  }


}

#endif