#ifndef INTERP
#define INTERP

#include <linSolve/MatX>
#include <cmath>

class Interp {
public:
  // Matrix for trilinear coordinate transformation
  MatX<double> AI; 

  Interp() {
    // initialize matrix; 
    AI.resize(8); 
    AI = { { 1, 0, 0, 0, 0, 0, 0, 0}, 
	   {-1, 1, 0, 0, 0, 0, 0, 0},
           {-1, 0, 0, 1, 0, 0, 0, 0},
           {-1, 0, 0, 0, 1, 0, 0, 0},
           { 1,-1, 1,-1, 0, 0, 0, 0},
           { 1,-1, 0, 0,-1, 1, 0, 0},
           { 1, 0, 0,-1,-1, 0, 0, 1},
           {-1, 1,-1, 1, 1,-1, 1,-1} };
  }
  
  // Coefficients are in the form of tri-linear interpolation
  //      8 values based on hexa-ordering should be given
  VecX<double> getCoef(VecX<double> &phi) {
    return AI*phi;
  };
  
  // Function itself 
  double linFunc(Vec3 xhat, VecX<double> &coef) {
    return coef[0] + coef[1]*xhat.x() + coef[2]*xhat.y() + coef[3]*xhat.z()
      + coef[4]*xhat.x()*xhat.y() + coef[5]*xhat.x()*xhat.z()
      + coef[6]*xhat.y()*xhat.z() + coef[7]*xhat.x()*xhat.y()*xhat.z(); 
  }

  // Partial derivation with respect to xhat
  double linDX(Vec3 xhat, VecX<double> &coef) {
    return coef[1] + coef[4]*xhat.y() + coef[5]*xhat.z() + coef[7]*xhat.y()*xhat.z();
  }
  
  // Partial derivation with respect to yhat
  double linDY(Vec3 xhat, VecX<double> &coef) {
      return coef[2] + coef[4]*xhat.x() + coef[6]*xhat.z() + coef[7]*xhat.x()*xhat.z(); 
  }

  // Partial derivation with respect to zhat
  double linDZ(Vec3 xhat, VecX<double> &coef) { 
      return coef[3] + coef[5]*xhat.x() + coef[6]*xhat.y() + coef[7]*xhat.x()*xhat.y(); 
  }

  Vec3 findXhat(Vec3 x, VecX<double> &xcoef, VecX<double> &ycoef, VecX<double> &zcoef) {
    Vec3 xhat(0, 0, 0); //INITIAL GUESS;
    Vec3 dx(0, 0, 0);  // needed if not updated!

    while (true) {
      double dxx = linDX(xhat, xcoef);
      double dxy = linDY(xhat, xcoef); 
      double dxz = linDZ(xhat, xcoef);

      if (std::abs(dxx) > 1e-10) {
	dx[0] = (x[0] - linFunc(xhat, xcoef) - dx[1]*dxy - dx[2]*dxz)/dxx; 
      }

      auto dyx = linDX(xhat, ycoef); 
      auto dyy = linDY(xhat, ycoef); 
      auto dyz = linDZ(xhat, ycoef); 

      if (std::abs(dyy) > 1e-10) { 
	dx[1] = (x[1] - linFunc(xhat, ycoef) - dx[0]*dyx - dx[2]*dyz)/dyy; 
      }

      auto dzx = linDX(xhat, zcoef); 
      auto dzy = linDY(xhat, zcoef); 
      auto dzz = linDZ(xhat, zcoef); 

      if (std::abs(dzz) > 1e-10) {
	dx[2] = (x[2] - linFunc(xhat, zcoef) - dx[0]*dzx - dx[1]*dzy)/dzz; 
      }
      xhat += dx; 

      if (abs(dx[0]) + abs(dx[1]) + abs(dx[2]) < 1e-6) break; 

    }
    xhat[0] = rint(1e6*xhat[0])*1e-6;
    xhat[1] = rint(1e6*xhat[1])*1e-6;
    xhat[2] = rint(1e6*xhat[2])*1e-6; 
    return xhat; 
  }

  double invDX(Vec3 xhat, VecX<double> &coef) {
    auto res = linDX(xhat, coef); 
    if (abs(res) < 1e-6) return 0; 
    else return 1./res; 
  }

  double invDY(Vec3 xhat, VecX<double> &coef) {
    auto res = linDY(xhat, coef); 
    if (abs(res) < 1e-6) return 0; 
    else return 1./res; 
  }

  double invDZ(Vec3 xhat, VecX<double> &coef) {
    auto res = linDZ(xhat, coef); 
    if (abs(res) < 1e-6) return 0; 
    else return 1./res; 
  }

};

#endif
