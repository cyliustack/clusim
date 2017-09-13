#!/bin/bash
if [[ "$HOSTNAME" == "TFnode0" ]]; then
	python /root/TensorflowBenchmarks/scripts/tf_cnn_benchmarks/tf_cnn_benchmarks.py --local_parameter_device=cpu --device=cpu --model=alexnet --variable_update=distributed_replicated --job_name=ps --ps_hosts=172.17.0.2:50000,172.17.0.3:50000 --worker_hosts=172.17.0.4:50001,172.17.0.5:50001,172.17.0.6:50001,172.17.0.7:50001,172.17.0.8:50001,172.17.0.9:50001 --task_index=0 --data_format=NHWC --force_gpu_compatible=false --num_batches=10 --display_every=1
fi
if [[ "$HOSTNAME" == "TFnode1" ]]; then
	python /root/TensorflowBenchmarks/scripts/tf_cnn_benchmarks/tf_cnn_benchmarks.py --local_parameter_device=cpu --device=cpu --model=alexnet --variable_update=distributed_replicated --job_name=ps --ps_hosts=172.17.0.2:50000,172.17.0.3:50000 --worker_hosts=172.17.0.4:50001,172.17.0.5:50001,172.17.0.6:50001,172.17.0.7:50001,172.17.0.8:50001,172.17.0.9:50001 --task_index=1  --data_format=NHWC --force_gpu_compatible=false --num_batches=10 --display_every=1
fi
if [[ "$HOSTNAME" == "TFnode2" ]]; then
	python /root/TensorflowBenchmarks/scripts/tf_cnn_benchmarks/tf_cnn_benchmarks.py --local_parameter_device=cpu --device=cpu --model=alexnet --variable_update=distributed_replicated --job_name=worker --ps_hosts=172.17.0.2:50000,172.17.0.3:50000 --worker_hosts=172.17.0.4:50001,172.17.0.5:50001,172.17.0.6:50001,172.17.0.7:50001,172.17.0.8:50001,172.17.0.9:50001 --task_index=0  --data_format=NHWC --force_gpu_compatible=false --num_batches=10 --display_every=1
fi
if [[ "$HOSTNAME" == "TFnode3" ]]; then
	python /root/TensorflowBenchmarks/scripts/tf_cnn_benchmarks/tf_cnn_benchmarks.py --local_parameter_device=cpu --device=cpu --model=alexnet --variable_update=distributed_replicated --job_name=worker --ps_hosts=172.17.0.2:50000,172.17.0.3:50000 --worker_hosts=172.17.0.4:50001,172.17.0.5:50001,172.17.0.6:50001,172.17.0.7:50001,172.17.0.8:50001,172.17.0.9:50001 --task_index=1  --data_format=NHWC --force_gpu_compatible=false --num_batches=10 --display_every=1
fi
if [[ "$HOSTNAME" == "TFnode4" ]]; then
	python /root/TensorflowBenchmarks/scripts/tf_cnn_benchmarks/tf_cnn_benchmarks.py --local_parameter_device=cpu --device=cpu --model=alexnet --variable_update=distributed_replicated --job_name=worker --ps_hosts=172.17.0.2:50000,172.17.0.3:50000 --worker_hosts=172.17.0.4:50001,172.17.0.5:50001,172.17.0.6:50001,172.17.0.7:50001,172.17.0.8:50001,172.17.0.9:50001 --task_index=2  --data_format=NHWC --force_gpu_compatible=false --num_batches=10 --display_every=1
fi
if [[ "$HOSTNAME" == "TFnode5" ]]; then
	python /root/TensorflowBenchmarks/scripts/tf_cnn_benchmarks/tf_cnn_benchmarks.py --local_parameter_device=cpu --device=cpu --model=alexnet --variable_update=distributed_replicated --job_name=worker --ps_hosts=172.17.0.2:50000,172.17.0.3:50000 --worker_hosts=172.17.0.4:50001,172.17.0.5:50001,172.17.0.6:50001,172.17.0.7:50001,172.17.0.8:50001,172.17.0.9:50001 --task_index=3  --data_format=NHWC --force_gpu_compatible=false --num_batches=10 --display_every=1
fi
if [[ "$HOSTNAME" == "TFnode6" ]]; then
	python /root/TensorflowBenchmarks/scripts/tf_cnn_benchmarks/tf_cnn_benchmarks.py --local_parameter_device=cpu --device=cpu --model=alexnet --variable_update=distributed_replicated --job_name=worker --ps_hosts=172.17.0.2:50000,172.17.0.3:50000 --worker_hosts=172.17.0.4:50001,172.17.0.5:50001,172.17.0.6:50001,172.17.0.7:50001,172.17.0.8:50001,172.17.0.9:50001 --task_index=4  --data_format=NHWC --force_gpu_compatible=false --num_batches=10 --display_every=1
fi
if [[ "$HOSTNAME" == "TFnode7" ]]; then
	python /root/TensorflowBenchmarks/scripts/tf_cnn_benchmarks/tf_cnn_benchmarks.py --local_parameter_device=cpu --device=cpu --model=alexnet --variable_update=distributed_replicated --job_name=worker --ps_hosts=172.17.0.2:50000,172.17.0.3:50000 --worker_hosts=172.17.0.4:50001,172.17.0.5:50001,172.17.0.6:50001,172.17.0.7:50001,172.17.0.8:50001,172.17.0.9:50001 --task_index=5  --data_format=NHWC --force_gpu_compatible=false --num_batches=10 --display_every=1
fi
