/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/gendoc/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef GENDOC_SYNTAX_LIBRARY_INCLUDED
#define GENDOC_SYNTAX_LIBRARY_INCLUDED

#include <gendoc/syntax/Project.hpp>
#include <gendoc/parsing/ParsingDomain.hpp>

namespace gendoc { namespace syntax {

void GenerateLibraryFile(Project* project, gendoc::parsing::ParsingDomain* parsingDomain);

} } // namespace cpg::syntax

#endif // GENDOC_SYNTAX_LIBRARY_INCLUDED
