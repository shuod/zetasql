//
// Copyright 2018 ZetaSQL Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#ifndef THIRD_PARTY_ZETASQL_ZETASQL_BASE_PATH_H_
#define THIRD_PARTY_ZETASQL_ZETASQL_BASE_PATH_H_

#include <initializer_list>
#include <string>
#include <utility>

#include "absl/strings/string_view.h"

// A set of file pathname manipulation routines.
// Calls to each of the following functions assume their input is
// well-formed (for some currently nebulous definition of the word).
//
// This collection is largely modeled on Python's os.path module.
// Filenames are restricted to ASCII characters.

namespace zetasql_base {

namespace internal {
// Not part of the public API.
std::string JoinPathImpl(bool honor_abs,
                    std::initializer_list<absl::string_view> paths);
}  // namespace internal

// Join multiple paths together.
// JoinPath and JoinPathRespectAbsolute have slightly different semantics.
// JoinPath unconditionally joins all paths together, whereas
// JoinPathRespectAbsolute ignores any segments prior to the last absolute
// path.  For example:
//
//  Arguments                  | JoinPath            | JoinPathRespectAbsolute
//  ---------------------------+---------------------+-----------------------
//  '/foo', 'bar'              | /foo/bar            | /foo/bar
//  '/foo/', 'bar'             | /foo/bar            | /foo/bar
//  '/foo', '/bar'             | /foo/bar            | /bar
//  '/foo', '/bar', '/baz'     | /foo/bar/baz        | /baz
//
// All paths will be treated as relative paths, regardless of whether or not
// they start with a leading '/'.  That is, all paths will be concatenated
// together, with the appropriate path separator inserted in between.
// Arguments must be convertible to absl::string_view.
//
// Usage:
// std::string path = zetasql_base::JoinPath("/cns", dirname, filename);
// std::string path = zetasql_base::JoinPath(FLAGS_test_srcdir, filename);
//
// 0, 1, 2-path specializations exist to optimize common cases.
inline std::string JoinPath() { return std::string(); }
inline std::string JoinPath(absl::string_view path) {
  return std::string(path.data(), path.size());
}
std::string JoinPath(absl::string_view path1, absl::string_view path2);
template <typename... T>
inline std::string JoinPath(absl::string_view path1, absl::string_view path2,
                       absl::string_view path3, const T&... args) {
  return internal::JoinPathImpl(false, {path1, path2, path3, args...});
}

// Join multiple paths together, respecting intermediate absolute paths.
// All paths will be joined together, but if any of the paths is absolute
// (as defined by IsAbsolutePath()), all prior path segments will be ignored.
// This is the behavior of the old zetasql_base::JoinPath().
// Arguments must be convertible to absl::string_view.
//
// Usage:
// std::string path = zetasql_base::JoinPathRespectAbsolute("/f", dirname, filename);
template <typename... T>
inline std::string JoinPathRespectAbsolute(const T&... args) {
  return internal::JoinPathImpl(true, {args...});
}

// Return true if path is absolute.
bool IsAbsolutePath(absl::string_view path);

// If path is non-empty and doesn't already end with a slash, append one
// to the end.
std::string AddSlash(absl::string_view path);

// Returns the part of the path before the final "/", EXCEPT:
// * If there is a single leading "/" in the path, the result will be the
//   leading "/".
// * If there is no "/" in the path, the result is the empty prefix of the
//   input std::string.
absl::string_view Dirname(absl::string_view path);

// Return the parts of the path, split on the final "/".  If there is no
// "/" in the path, the first part of the output is empty and the second
// is the input. If the only "/" in the path is the first character, it is
// the first part of the output.
std::pair<absl::string_view, absl::string_view> SplitPath(
    absl::string_view path);

// Returns the part of the path after the final "/".  If there is no
// "/" in the path, the result is the same as the input.
// Note that this function's behavior differs from the Unix basename
// command if path ends with "/". For such paths, this function returns the
// empty std::string.
absl::string_view Basename(absl::string_view path);

}  // namespace zetasql_base

#endif  // THIRD_PARTY_ZETASQL_ZETASQL_BASE_PATH_H_
