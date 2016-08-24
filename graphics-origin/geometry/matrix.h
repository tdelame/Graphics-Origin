/* Created on: 2015-08-15
 *     Author: T.Delame (tdelame@gmail.com)
 */
# ifndef GRAPHICS_ORIGIN_MATRIX_H_
# define GRAPHICS_ORIGIN_MATRIX_H_

# include "../graphics_origin.h"
# include "vec.h"

# include <glm/glm.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtx/transform.hpp>
# include <glm/gtc/type_ptr.hpp>
# include <glm/gtc/matrix_access.hpp>
BEGIN_GO_NAMESPACE

typedef glm::tmat4x4< real, glm::highp> mat4;
typedef glm::tmat3x3< real, glm::highp> mat3;

typedef glm::tmat4x4< gl_real, glm::highp> gl_mat4;
typedef glm::tmat3x3< gl_real, glm::highp> gl_mat3;

END_GO_NAMESPACE

# include <iosfwd>

namespace std {

  GO_API std::ostream& operator<<(std::ostream& os, const GO_NAMESPACE::mat3& v);
  GO_API std::ostream& operator<<(std::ostream& os, const GO_NAMESPACE::mat4& v);
  GO_API std::ostream& operator<<(std::ostream& os, const GO_NAMESPACE::gl_mat3& v);
  GO_API std::ostream& operator<<(std::ostream& os, const GO_NAMESPACE::gl_mat4& v);

  GO_API std::istream& operator>>(std::istream& is, GO_NAMESPACE::mat3& v);
  GO_API std::istream& operator>>(std::istream& is, GO_NAMESPACE::mat4& v);
  GO_API std::istream& operator>>(std::istream& is, GO_NAMESPACE::gl_mat3& v);
  GO_API std::istream& operator>>(std::istream& is, GO_NAMESPACE::gl_mat4& v);

}

# endif /* PROJECT_NAMESPACE_MATRIX_H_ */
