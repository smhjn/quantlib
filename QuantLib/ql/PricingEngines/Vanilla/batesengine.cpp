/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2005 Klaus Spanderen

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/reference/license.html>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include <ql/PricingEngines/Vanilla/batesengine.hpp>
#include <ql/Instruments/payoffs.hpp>

#if !defined(QL_PATCH_MSVC6) && !defined(QL_PATCH_BORLAND)

namespace QuantLib {

    BatesEngine::BatesEngine(const boost::shared_ptr<BatesModel> & model,
                             Size integrationOrder)
    : AnalyticHestonEngine(model, integrationOrder) { }

    std::complex<Real> BatesEngine::jumpDiffusionTerm(Real phi,
                                                      Time t, Size j) const {
        boost::shared_ptr<BatesModel> batesModel =
            boost::dynamic_pointer_cast<BatesModel>(model_);

        const Real nu_     = batesModel->nu();
        const Real delta2_ = 0.5*batesModel->delta()*batesModel->delta();
        const Real lambda_ = batesModel->lambda();
        const Real i       = (j == 1)? 1.0 : 0.0;
        const std::complex<Real> g(i, phi);

        return t*lambda_*(std::exp(nu_*g + delta2_*g*g) - 1.0
                          -g*(std::exp(nu_+delta2_) - 1.0));
    }


    BatesDetJumpEngine::BatesDetJumpEngine(
        const boost::shared_ptr<BatesDetJumpModel> & model,
        Size integrationOrder)
    : BatesEngine(model, integrationOrder) { }

    std::complex<Real> BatesDetJumpEngine::jumpDiffusionTerm(
        Real phi, Time t, Size j) const {

        const std::complex<Real> l =
            BatesEngine::jumpDiffusionTerm(phi, t, j);

        boost::shared_ptr<BatesDetJumpModel> batesDetJumpModel =
            boost::dynamic_pointer_cast<BatesDetJumpModel>(model_);

        const Real lambda      = batesDetJumpModel->lambda();
        const Real kappaLambda = batesDetJumpModel->kappaLambda();
        const Real thetaLambda = batesDetJumpModel->thetaLambda();

        return (kappaLambda*t - 1.0 + std::exp(-kappaLambda*t))
            * thetaLambda*l/(kappaLambda*t*lambda)
            + (1.0 - std::exp(-kappaLambda*t))*l/(kappaLambda*t);
    }


    BatesDoubleExpEngine::BatesDoubleExpEngine(
        const boost::shared_ptr<BatesDoubleExpModel> & model,
        Size integrationOrder)
    : AnalyticHestonEngine(model, integrationOrder) { }


    std::complex<Real> BatesDoubleExpEngine::jumpDiffusionTerm(
        Real phi, Time t, Size j) const {
        boost::shared_ptr<BatesDoubleExpModel> batesDoubleExpModel =
            boost::dynamic_pointer_cast<BatesDoubleExpModel>(model_);

        const Real p_     = batesDoubleExpModel->p();
        const Real q_     = 1.0-p_;
        const Real nuDown_= batesDoubleExpModel->nuDown();
        const Real nuUp_  = batesDoubleExpModel->nuUp();
        const Real lambda_= batesDoubleExpModel->lambda();
        const Real i      = (j == 1)? 1.0 : 0.0;
        const std::complex<Real> g(i, phi);

        return t*lambda_*(p_/(1.0-g*nuUp_) + q_/(1.0+g*nuDown_) - 1.0
                          - g*(p_/(1-nuUp_) + q_/(1+nuDown_)-1));
    }

    BatesDoubleExpDetJumpEngine::BatesDoubleExpDetJumpEngine(
        const boost::shared_ptr<BatesDoubleExpDetJumpModel> & model,
        Size integrationOrder)
    : BatesDoubleExpEngine(model, integrationOrder) { }

    std::complex<Real> BatesDoubleExpDetJumpEngine::jumpDiffusionTerm(
        Real phi, Time t, Size j) const {
        const std::complex<Real> l =
            BatesDoubleExpEngine::jumpDiffusionTerm(phi, t, j);

        boost::shared_ptr<BatesDoubleExpDetJumpModel> doubleExpDetJumpModel
            = boost::dynamic_pointer_cast<BatesDoubleExpDetJumpModel>(model_);

        const Real lambda      = doubleExpDetJumpModel->lambda();
        const Real kappaLambda = doubleExpDetJumpModel->kappaLambda();
        const Real thetaLambda = doubleExpDetJumpModel->thetaLambda();

        return (kappaLambda*t - 1.0 + std::exp(-kappaLambda*t))
            * thetaLambda*l/(kappaLambda*t*lambda)
            + (1.0 - std::exp(-kappaLambda*t))*l/(kappaLambda*t);
    }

}


#endif

