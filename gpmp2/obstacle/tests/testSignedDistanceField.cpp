/**
*  @file testSDFutils.cpp
*  @author Jing Dong
**/

#include <CppUnitLite/TestHarness.h>
#include <gtsam/base/numericalDerivative.h>

#include <gtsam/base/Matrix.h>

#include <gpmp2/obstacle/SignedDistanceField.h>

#include <iostream>

using namespace std;
using namespace gtsam;
using namespace gpmp2;


double sdf_wrapper(const SignedDistanceField& field, const Point3& p) {
  return field.getSignedDistance(p);
}

/* ************************************************************************** */
TEST(SDFutils, test) {
  // data
  vector<Matrix> data;
  data.push_back((Matrix(5, 5) <<
      1.7321, 1.4142, 1.4142, 1.4142, 1.7321,
      1.4142, 1, 1, 1, 1.4142,
      1.4142, 1, 1, 1, 1.4142,
      1.4142, 1, 1, 1, 1.4142,
      1.7321, 1.4142, 1.4142, 1.4142, 1.7321).finished());
  data.push_back((Matrix(5, 5) <<
      1.4142, 1, 1, 1, 1.4142,
      1, 0, 0, 0, 1,
      1, 0, 0, 0, 1,
      1, 0, 0, 0, 1,
      1.4142, 1, 1, 1,4142).finished());
  data.push_back((Matrix(5, 5) <<
      1.7321, 1.4142, 1.4142, 1.4142, 1.7321,
      1.4142, 1, 1, 1, 1.4142,
      1.4142, 1, 1, 1, 1.4142,
      1.4142, 1, 1, 1, 1.4142,
      1.7321, 1.4142, 1.4142, 1.4142, 1.7321).finished());
  Point3 origin(-0.2, -0.2, -0.1);
  double cell_size = 0.1;

  // constructor
  SignedDistanceField field(origin, cell_size, data);
  EXPECT_LONGS_EQUAL(5, field.x_count());
  EXPECT_LONGS_EQUAL(5, field.y_count());
  EXPECT_LONGS_EQUAL(3, field.z_count());
  EXPECT_DOUBLES_EQUAL(0.1, field.cell_size(), 1e-9);
  EXPECT(assert_equal(origin, field.origin()));

  // access
  SignedDistanceField::float_index idx;
  idx = field.convertPoint3toCell(Point3(0, 0, 0));
  EXPECT_DOUBLES_EQUAL(2, idx.get<0>(), 1e-9);
  EXPECT_DOUBLES_EQUAL(2, idx.get<1>(), 1e-9);
  EXPECT_DOUBLES_EQUAL(1, idx.get<2>(), 1e-9);
  EXPECT_DOUBLES_EQUAL(0, field.signed_distance(idx), 1e-9)
  idx = field.convertPoint3toCell(Point3(0.18, -0.18, 0.07));   // tri-linear interpolation
  EXPECT_DOUBLES_EQUAL(0.2, idx.get<0>(), 1e-9);
  EXPECT_DOUBLES_EQUAL(3.8, idx.get<1>(), 1e-9);
  EXPECT_DOUBLES_EQUAL(1.7, idx.get<2>(), 1e-9);
  EXPECT_DOUBLES_EQUAL(1.488288, field.signed_distance(idx), 1e-9)
  idx = boost::make_tuple(1.0, 2.0, 3.0);
  EXPECT(assert_equal(Point3(0.0, -0.1, 0.2), field.convertCelltoPoint3(idx)));

  // gradient
  Vector3 grad_act, grad_exp;
  Point3 p;
  p = Point3(-0.13, -0.14, 0.06);
  field.getSignedDistance(p, grad_act);
  grad_exp = numericalDerivative11(std::function<double(const Point3&)>(
      boost::bind(sdf_wrapper, field, _1)), p, 1e-6);
  EXPECT(assert_equal(grad_exp, grad_act, 1e-6));

  p = Point3(0.18, 0.12, 0.01);
  field.getSignedDistance(p, grad_act);
  grad_exp = numericalDerivative11(std::function<double(const Point3&)>(
      boost::bind(sdf_wrapper, field, _1)), p, 1e-6);
  EXPECT(assert_equal(grad_exp, grad_act, 1e-6));
}

/* ************************************************************************** */
/* main function */
int main() {
  TestResult tr;
  return TestRegistry::runAllTests(tr);
}


