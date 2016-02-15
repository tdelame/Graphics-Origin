/* Created on: 2015-08-15
 *     Author: T.Delame (tdelame@gmail.com)
 */
# ifndef GRAPHICS_ORIGIN_MATRIX_H_
# define GRAPHICS_ORIGIN_MATRIX_H_

# include "../graphics_origin.h"

# include <glm/glm.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtc/type_ptr.hpp>
# include <glm/gtc/matrix_access.hpp>
BEGIN_GO_NAMESPACE

typedef glm::tmat4x4< real, glm::highp> mat4;
typedef glm::tmat3x3< real, glm::highp> mat3;

typedef glm::tmat4x4< gpu_real, glm::highp> gpu_mat4;
typedef glm::tmat3x3< gpu_real, glm::highp> gpu_mat3;

END_GO_NAMESPACE

# include <boost/serialization/nvp.hpp>
# include "vec.h"
namespace boost {
  namespace serialization {
    template< class Archive >
    void serialize( Archive & ar, GO_NAMESPACE::mat4& v, const unsigned int version )
    {
      (void)version;
      ar & make_nvp("col0", v[0]);
      ar & make_nvp("col1", v[1]);
      ar & make_nvp("col2", v[2]);
      ar & make_nvp("col3", v[3]);
    }

    template< class Archive >
    void serialize( Archive & ar, GO_NAMESPACE::mat3& v, const unsigned int version )
    {
      (void)version;
      ar & make_nvp("col0", v[0]);
      ar & make_nvp("col1", v[1]);
      ar & make_nvp("col2", v[2]);
    }

    template< class Archive >
    void serialize( Archive & ar, GO_NAMESPACE::gpu_mat4& v, const unsigned int version )
    {
      (void)version;
      ar & make_nvp("col0", v[0]);
      ar & make_nvp("col1", v[1]);
      ar & make_nvp("col2", v[2]);
      ar & make_nvp("col3", v[3]);
    }

    template< class Archive >
    void serialize( Archive & ar, GO_NAMESPACE::gpu_mat3& v, const unsigned int version )
    {
      (void)version;
      ar & make_nvp("col0", v[0]);
      ar & make_nvp("col1", v[1]);
      ar & make_nvp("col2", v[2]);
    }
  }
}

# include <iosfwd>

namespace std {

  std::ostream& operator<<(std::ostream& os, const GO_NAMESPACE::mat3& v);
  std::ostream& operator<<(std::ostream& os, const GO_NAMESPACE::mat4& v);
  std::ostream& operator<<(std::ostream& os, const GO_NAMESPACE::gpu_mat3& v);
  std::ostream& operator<<(std::ostream& os, const GO_NAMESPACE::gpu_mat4& v);

  std::istream& operator>>(std::istream& is, GO_NAMESPACE::mat3& v);
  std::istream& operator>>(std::istream& is, GO_NAMESPACE::mat4& v);
  std::istream& operator>>(std::istream& is, GO_NAMESPACE::gpu_mat3& v);
  std::istream& operator>>(std::istream& is, GO_NAMESPACE::gpu_mat4& v);

}

# endif /* PROJECT_NAMESPACE_MATRIX_H_ */
