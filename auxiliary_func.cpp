#include "auxiliary_func.hpp"

namespace PrositAux {
    RowVectorXd stat(const MatrixXd & A) throw (Exc) {
        if(A.rows()!=A.cols())
            throw Exc("Function Stat: The matrix A has to be square");
        
        VectorXd e(A.rows());
        MatrixXd B(A.rows(), A.cols()+1);
        MatrixXd P;
        e.setOnes();
        
        B.block(0, 0, A.rows(), A.cols()) = A - MatrixXd::Identity(A.rows(), A.cols());
        B.block(0, A.cols(), e.rows(), e.cols()) = e;
        
        RowVectorXd y(A.rows()+1);
        y.setZero();
        y(A.rows()) = 1;
        pseudoInverse<MatrixXd>(B,P);
        return y*P;
    }
}
