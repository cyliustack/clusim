#!/bin/bash
#~/benchmark/tools/ssh_copy_id_to_all.sh 4
#~/benchmark/tools/genhosts.sh 4
#mpirun -f hosts.txt benchmark/mpi/gemm/gemm 1000 1000 1000
if [[ "$HOSTNAME" == "TFnode0" ]]; then
	~/benchmark/tools/ssh_copy_id_to_all.sh 4
	~/benchmark/tools/genhosts.sh 4
	python /root/TensorflowBenchmarks/scripts/tf_cnn_benchmarks/tf_cnn_benchmarks.py --local_parameter_device=cpu --device=cpu --model=resnet50 --variable_update=distributed_replicated --job_name=ps --ps_hosts=172.17.0.2:50000 --worker_hosts=172.17.0.3:50001,172.17.0.4:50001,172.17.0.5:50001 --task_index=0 --data_format=NHWC --force_gpu_compatible=false --num_batches=10 --display_every=1
fi
if [[ "$HOSTNAME" == "TFnode1" ]]; then
	#statements
	python /root/TensorflowBenchmarks/scripts/tf_cnn_benchmarks/tf_cnn_benchmarks.py --local_parameter_device=cpu --device=cpu --model=resnet50 --variable_update=distributed_replicated --job_name=worker --ps_hosts=172.17.0.2:50000 --worker_hosts=172.17.0.3:50001,172.17.0.4:50001,172.17.0.5:50001 --task_index=0  --data_format=NHWC --force_gpu_compatible=false --num_batches-10 --display_every=1
fi
if [[ "$HOSTNAME" == "TFnode2" ]]; then
	#statements
	python /root/TensorflowBenchmarks/scripts/tf_cnn_benchmarks/tf_cnn_benchmarks.py --local_parameter_device=cpu --device=cpu --model=resnet50 --variable_update=distributed_replicated --job_name=worker --ps_hosts=172.17.0.2:50000 --worker_hosts=172.17.0.3:50001,172.17.0.4:50001,172.17.0.5:50001 --task_index=1  --data_format=NHWC --force_gpu_compatible=false --num_batches=10 --display_every=1
fi
if [[ "$HOSTNAME" == "TFnode3" ]]; then
	#statements
	python /root/TensorflowBenchmarks/scripts/tf_cnn_benchmarks/tf_cnn_benchmarks.py --local_parameter_device=cpu --device=cpu --model=resnet50 --variable_update=distributed_replicated --job_name=worker --ps_hosts=172.17.0.2:50000 --worker_hosts=172.17.0.3:50001,172.17.0.4:50001,172.17.0.5:50001 --task_index=2  --data_format=NHWC --force_gpu_compatible=false --num_batches=10 --display_every=1
fi
