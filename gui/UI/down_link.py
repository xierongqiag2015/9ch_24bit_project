import sqlalchemy as sa
from models import Base

class DownLink(Base):
    __tablename__ = 'down_link'
    id = sa.Column('id', sa.Integer(), primary_key=True, autoincrement=True)
    ip = sa.Column('ip', sa.String(length=64), nullable=False)
    manual_samp_time = sa.Column('manual_samp_time', sa.String(length=10), nullable=True)
    start_time = sa.Column('start_time', sa.DateTime(), nullable=True)
    end_time = sa.Column('end_time', sa.DateTime(), nullable=True)
    cmd = sa.Column('cmd', sa.String(length=10), nullable=True)
