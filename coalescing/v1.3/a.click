FromDPDKDevice(0000:02:00.0, N_QUEUES 1, MAXTHREADS 1, MODE none)
	->Print('x')
	->EtherRewrite(04:70:00:00:00:10, 04:70:00:00:03:10)
	->ToDPDKDevice(0000:03:00.0, N_QUEUES 1);
FromDPDKDevice(0000:03:00.0, N_QUEUES 1, MAXTHREADS 1, MODE none)
	->Print('y')
	->EtherRewrite(04:70:00:00:00:02, 04:70:00:00:01:01)
	->ToDPDKDevice(0000:02:00.0, N_QUEUES 1);
