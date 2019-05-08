# build the root files out of the STAR and PHENIX MB files
phenix=../../data/phenix_pikp_data/piKp_phenix.root
star=../../data/star_pip_data/pip_STAR.root
rootcp --recreate $phenix:K_0_100   fn_weight.root:K
rootcp            $phenix:Kn_0_100  fn_weight.root:Kn
rootcp            $star:p_MB        fn_weight.root:p
rootcp            $star:pbar_MB     fn_weight.root:pbar
rootcp            $star:pi_MB       fn_weight.root:pi
rootcp            $star:pin_MB      fn_weight.root:pin

# run the analysis script
./fn_weight.cc
