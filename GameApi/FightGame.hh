
#include "Effect.hh"

struct State
{
public:
  Point pos;
  Vector head;
  Vector body_center;
  Vector left_leg;
  Vector right_leg;
  Vector left_arm;
  Vector right_arm;
};

void create_obj(State &sm, FaceStore &s)
{
  CubeElem cube_a;
  MatrixElem cube(cube_a, Matrix::Scale(100.0,100.0,100.0));
  Point head_pos = sm.pos + sm.head;
  MatrixElem head(cube, Matrix::Translate(head_pos.x, head_pos.y, head_pos.z));
  Point body_pos = sm.pos + sm.body_center;
  MatrixElem body(cube, Matrix::Translate(body_pos.x, body_pos.y, body_pos.z));
  Point left_leg_pos = sm.pos + sm.left_leg;
  MatrixElem left_leg(cube, Matrix::Translate(left_leg_pos.x, left_leg_pos.y, left_leg_pos.z));
  Point right_leg_pos = sm.pos + sm.right_leg;
  MatrixElem right_leg(cube, Matrix::Translate(right_leg_pos.x, right_leg_pos.y, right_leg_pos.z));  

  Point left_arm_pos = sm.pos + sm.left_arm;
  MatrixElem left_arm(cube, Matrix::Translate(left_arm_pos.x, left_arm_pos.y, left_arm_pos.z));
  Point right_arm_pos = sm.pos + sm.right_arm;

  MatrixElem right_arm(cube, Matrix::Translate(head_pos.x, head_pos.y, head_pos.z));

  
}
