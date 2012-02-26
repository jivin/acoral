#include "acoral.h"
#include"../include/test.h"
ACORAL_RM_THREAD test(acoral_u32 args){
	acoral_print("task:%d on cpu:%d\n",args,acoral_current_cpu);
}

void test_rm_init()
{
	acoral_print("RM test Demo is Running\n");
	acoral_rm_policy_data_t policy_data;
	policy_data.t=10;
	policy_data.e=2;
	acoral_create_thread_ext(test, 352,(acoral_u32)1,"Rm",NULL,ACORAL_SCHED_POLICY_RM,&policy_data);
	policy_data.t=15;
	policy_data.e=2;
	acoral_create_thread_ext(test, 352,(acoral_u32)2,"Rm",NULL,ACORAL_SCHED_POLICY_RM,&policy_data);
	policy_data.t=10;
	policy_data.e=2;
	acoral_create_thread_ext(test, 352,(acoral_u32)3,"Rm",NULL,ACORAL_SCHED_POLICY_RM,&policy_data);
	policy_data.t=18;
	policy_data.e=3;
	acoral_create_thread_ext(test, 352,(acoral_u32)4,"RM",NULL,ACORAL_SCHED_POLICY_RM,&policy_data);
	policy_data.t=10;
	policy_data.e=2;
	acoral_create_thread_ext(test, 352,(acoral_u32)5,"RM",NULL,ACORAL_SCHED_POLICY_RM,&policy_data);
	policy_data.t=10;
	policy_data.e=2;
	acoral_create_thread_ext(test, 352,(acoral_u32)6,"Rm",NULL,ACORAL_SCHED_POLICY_RM,&policy_data);
	policy_data.t=10;
	policy_data.e=2;
	acoral_create_thread_ext(test, 352,(acoral_u32)7,"Rm",NULL,ACORAL_SCHED_POLICY_RM,&policy_data);
	policy_data.t=10;
	policy_data.e=2;
	acoral_create_thread_ext(test, 352,(acoral_u32)8,"Rm",NULL,ACORAL_SCHED_POLICY_RM,&policy_data);
	policy_data.t=10;
	policy_data.e=2;
	acoral_create_thread_ext(test, 352,(acoral_u32)9,"RM",NULL,ACORAL_SCHED_POLICY_RM,&policy_data);
	policy_data.t=10;
	policy_data.e=2;
	acoral_create_thread_ext(test, 352,(acoral_u32)10,"Rm",NULL,ACORAL_SCHED_POLICY_RM,&policy_data);
	policy_data.t=15;
	policy_data.e=2;
	acoral_create_thread_ext(test, 352,(acoral_u32)11,"Rm",NULL,ACORAL_SCHED_POLICY_RM,&policy_data);
	policy_data.t=10;
	policy_data.e=2;
	acoral_create_thread_ext(test, 352,(acoral_u32)12,"Rm",NULL,ACORAL_SCHED_POLICY_RM,&policy_data);
	policy_data.t=18;
	policy_data.e=3;
	acoral_create_thread_ext(test, 352,(acoral_u32)13,"Rm",NULL,ACORAL_SCHED_POLICY_RM,&policy_data);
	policy_data.t=10;
	policy_data.e=2;
	acoral_create_thread_ext(test, 352,(acoral_u32)14,"Rm",NULL,ACORAL_SCHED_POLICY_RM,&policy_data);
	policy_data.t=10;
	policy_data.e=2;
	acoral_create_thread_ext(test, 352,(acoral_u32)15,"Rm",NULL,ACORAL_SCHED_POLICY_RM,&policy_data);
	policy_data.t=10;
	policy_data.e=2;
	acoral_create_thread_ext(test, 352,(acoral_u32)16,"RM",NULL,ACORAL_SCHED_POLICY_RM,&policy_data);
	policy_data.t=10;
	policy_data.e=2;
	acoral_create_thread_ext(test, 352,(acoral_u32)17,"Rm",NULL,ACORAL_SCHED_POLICY_RM,&policy_data);
	policy_data.t=10;
	policy_data.e=2;
	acoral_create_thread_ext(test, 352,(acoral_u32)18,"Rm",NULL,ACORAL_SCHED_POLICY_RM,&policy_data);

}

TEST_CALL(test_rm_init)
