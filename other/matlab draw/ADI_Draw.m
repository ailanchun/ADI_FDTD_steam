 A=load('E:\ADI_FDTD\ADI_FDTD_steam\result\temp_matle_7.txt'); %��sj.txt�ļ����ݣ���ֵ��A����

 B=load('E:\ADI_FDTD\Source-test\source_test\result\source08.txt'); %��sj.txt�ļ����ݣ���ֵ��A����
 
 rx1=A(:,1); 
 
 ry1=A(:,2);
 ry2=A(:,3);
 ry3=A(:,4);
 ry4=A(:,5);
 ry5=A(:,6);
 ry6=A(:,7);
 
 sx1=B(:,1); 

 sy1=B(:,2);
 sy2=B(:,3);
 sy3=B(:,4);
 sy4=B(:,5);
 sy5=B(:,6);
 sy6=B(:,7);
 
%�糡��� 
 subplot(2,2,1);
 plot(rx1,ry1,'LineWidth',2,'Color',[1 0 0],'DisplayName','Ex');
 hold on
 plot(rx1,ry2,'LineWidth',2,'Color',[0.0784313753247261 0.168627455830574 0.549019634723663],'DisplayName','Ey');
 hold on
 plot(rx1,ry3,'LineWidth',2,'Color',[0 1 0],'DisplayName','Ez');
 hold on
 ylabel('���');
% ���� xlabel
xlabel('ʱ�䲽��(1.0*e-12s)');
% ���� title
title('�糡������','FontSize',12,'FontName','Microsoft YaHei UI');
% ���� legend
 legend('show');
 grid on
 hold on
 
 %�ų����
 subplot(2,2,2); 
 plot(rx1,ry4,'LineWidth',2,'Color',[1 0 0],'DisplayName','Hx');
 hold on
 plot(rx1,ry5,'LineWidth',2,'Color',[0.0784313753247261 0.168627455830574 0.549019634723663],'DisplayName','Hy');
 hold on
 plot(rx1,ry6,'LineWidth',2,'Color',[0 1 0],'DisplayName','Hz');
 hold on
 ylabel('���');
% ���� xlabel
xlabel('ʱ�䲽��(1.0*e-12s)');
% ���� title
title('�ų�������','FontSize',12,'FontName','Microsoft YaHei UI');
% ���� legend
legend('show');
grid on
hold on

 %Դ�糡������ 
 subplot(2,2,3);

 plot(sx1,sy1,'LineWidth',2,'Color',[1 0 0],'DisplayName','Ex');
 hold on
 plot(sx1,sy2,'LineWidth',2,'Color',[0.0784313753247261 0.168627455830574 0.549019634723663],'DisplayName','Ey');
 hold on
 plot(sx1,sy3,'LineWidth',2,'Color',[0 1 0],'DisplayName','Ez');
 hold on
 % ���� ylabel
ylabel('���');
% ���� xlabel
xlabel('ʱ�䲽��(1.0*e-12s)');
% ���� title
title('Դ�糡','FontSize',12,'FontName','Microsoft YaHei UI');
% ���� legend
legend('show');
grid on


 %Դ�ų������� 
 subplot(2,2,4);

 plot(sx1,sy4,'LineWidth',2,'Color',[1 0 0],'DisplayName','Hx');
 hold on
 plot(sx1,sy5,'LineWidth',2,'Color',[0.0784313753247261 0.168627455830574 0.549019634723663],'DisplayName','Hy');
 hold on
 plot(sx1,sy6,'LineWidth',2,'Color',[0 1 0],'DisplayName','Hz');
 hold on
 % ���� ylabel
ylabel('���');
% ���� xlabel
xlabel('ʱ�䲽��(1.0*e-12s)');
% ���� title
title('Դ�ų�','FontSize',12,'FontName','Microsoft YaHei UI');
% ���� legend
legend('show');
grid on
 hold on

 

