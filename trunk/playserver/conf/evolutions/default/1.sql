# --- Created by Ebean DDL
# To stop Ebean DDL generation, remove this comment and start using Evolutions

# --- !Ups

create table chat_message (
  id                        bigint not null,
  from_id                   bigint,
  to_id                     bigint,
  call_id                   varchar(255),
  transac_id                varchar(255),
  json_content              clob,
  created_at                timestamp,
  updated_at                timestamp,
  status                    integer,
  constraint ck_chat_message_status check (status in (0,1,2,3)),
  constraint pk_chat_message primary key (id))
;

create table contact (
  id                        bigint not null,
  type                      integer,
  first_name                varchar(255),
  last_name                 varchar(255),
  friendly_name             varchar(255),
  user_id                   bigint,
  constraint ck_contact_type check (type in (0,1,2)),
  constraint pk_contact primary key (id))
;

create table developer (
  id                        bigint not null,
  email                     varchar(255) not null,
  password                  varchar(255),
  friendly_name             varchar(255),
  license_key               varchar(255),
  constraint uq_developer_email unique (email),
  constraint pk_developer primary key (id))
;

create table ice_server (
  id                        bigint not null,
  protocol                  integer,
  enable_stun               boolean default true not null,
  enable_turn               boolean default false not null,
  host                      varchar(255) not null,
  port                      smallint not null,
  login                     varchar(255),
  password                  varchar(255),
  developer_id              bigint,
  constraint ck_ice_server_protocol check (protocol in (0,1,2)),
  constraint pk_ice_server primary key (id))
;

create table phone (
  id                        bigint not null,
  number                    varchar(255) not null,
  contact_id                bigint,
  constraint pk_phone primary key (id))
;

create table sip_account (
  id                        bigint not null,
  address                   varchar(255),
  user_id                   bigint,
  constraint pk_sip_account primary key (id))
;

create table user (
  id                        bigint not null,
  email                     varchar(255) not null,
  friendly_name             varchar(255),
  phone_number              varchar(255),
  icon                      varchar(255),
  password                  varchar(255),
  developer_id              bigint,
  constraint uq_user_email unique (email),
  constraint pk_user primary key (id))
;

create sequence chat_message_seq;

create sequence contact_seq;

create sequence developer_seq;

create sequence ice_server_seq;

create sequence phone_seq;

create sequence sip_account_seq;

create sequence user_seq;

alter table chat_message add constraint fk_chat_message_from_1 foreign key (from_id) references user (id) on delete restrict on update restrict;
create index ix_chat_message_from_1 on chat_message (from_id);
alter table chat_message add constraint fk_chat_message_to_2 foreign key (to_id) references user (id) on delete restrict on update restrict;
create index ix_chat_message_to_2 on chat_message (to_id);
alter table contact add constraint fk_contact_user_3 foreign key (user_id) references user (id) on delete restrict on update restrict;
create index ix_contact_user_3 on contact (user_id);
alter table ice_server add constraint fk_ice_server_developer_4 foreign key (developer_id) references developer (id) on delete restrict on update restrict;
create index ix_ice_server_developer_4 on ice_server (developer_id);
alter table phone add constraint fk_phone_contact_5 foreign key (contact_id) references contact (id) on delete restrict on update restrict;
create index ix_phone_contact_5 on phone (contact_id);
alter table sip_account add constraint fk_sip_account_user_6 foreign key (user_id) references user (id) on delete restrict on update restrict;
create index ix_sip_account_user_6 on sip_account (user_id);
alter table user add constraint fk_user_developer_7 foreign key (developer_id) references developer (id) on delete restrict on update restrict;
create index ix_user_developer_7 on user (developer_id);



# --- !Downs

SET REFERENTIAL_INTEGRITY FALSE;

drop table if exists chat_message;

drop table if exists contact;

drop table if exists developer;

drop table if exists ice_server;

drop table if exists phone;

drop table if exists sip_account;

drop table if exists user;

SET REFERENTIAL_INTEGRITY TRUE;

drop sequence if exists chat_message_seq;

drop sequence if exists contact_seq;

drop sequence if exists developer_seq;

drop sequence if exists ice_server_seq;

drop sequence if exists phone_seq;

drop sequence if exists sip_account_seq;

drop sequence if exists user_seq;

